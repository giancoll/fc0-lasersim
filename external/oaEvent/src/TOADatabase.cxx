//////////////////////////////////////////////////////////////
// $Id$
//
// Implement the class ND::TOADatabase which provides the user interface
// to the calibrations and geometry databases.  This class is a
// singleton.
//

#include <ctime>
#include <memory>
#include <map>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>

#include <TSystem.h>
#include <TTimeStamp.h>
#include <TDatabasePDG.h>
#include <TFile.h>
#include <TKey.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoPhysicalNode.h>

#include "TND280Event.hxx"
#include "TOADatabase.hxx"
#include "THATPadManager.hxx"
#include "TTPCPadManager.hxx"
#include "TEventFolder.hxx"
#include "TDigitManager.hxx"
#include "TGeomIdManager.hxx"
#include "SHA1.hxx"

ClassImp(ND::TOADatabase);

/// The static member pointer to the singleton.
ND::TOADatabase* ND::TOADatabase::fOADatabase = NULL;

namespace {
    typedef std::pair < std::time_t , ND::TSHAHashValue> HashValue;
    typedef std::vector< HashValue > Hash;

    bool hashCompare(const HashValue& lhs, const HashValue& rhs) {
        return lhs.first < rhs.first;
    }

    /// Implement a default GeometryLookup class.  This is used unless another
    /// one is explicitly registered.
    class DefaultGeometryLookup: public ND::TOADatabase::GeometryLookup {
        Hash fHashs;
    public:
        virtual ~DefaultGeometryLookup() {}
        // Open the context file and read the list of geometries.
        DefaultGeometryLookup() {
            // Generate the name of the file.
            // Geometry information is normally stored in ${OAEVENTROOT}/${OAEVENTCONFIG}/GEOMETRY.LIST
            // However an alternative is to keep it in ${OAEVENTGEOMETRYSTORE}/GEOMETRY.LIST. If this environment
            // variable is not empty it takes precedence. This requires GEOMETRY.LIST to be correctly maintained!
            //
            std::string geometryName(gSystem->Getenv("OAEVENTROOT"));
            if(gSystem->Getenv("OAEVENTGEOMETRYSTORE"))
            {
                std::string geometryStoreLocation(gSystem->Getenv("OAEVENTGEOMETRYSTORE"));
                if (!geometryStoreLocation.empty())
                    geometryName=geometryStoreLocation;
                else
                {
                    geometryName += "/";
                    geometryName += gSystem->Getenv("OAEVENTCONFIG");
                }
            }
            geometryName += "/GEOMETRY.LIST" ;
            // Attach the file to a stream.
            std::ifstream geoContext(geometryName.c_str());
            std::string line;
            while (std::getline(geoContext,line)) {
                std::size_t comment = line.find("#");
                std::string tmp = line;
                if (comment != std::string::npos) tmp.erase(comment);
                // The minimum valid date, time and hash is 22 characters.
                if (tmp.size()<20) continue;
                std::istringstream parser(tmp);
                int yr, mn, da, hr, m;
                unsigned int hash0 = 0;
                unsigned int hash1 = 0;
                unsigned int hash2 = 0;
                unsigned int hash3 = 0;
                unsigned int hash4 = 0;
                char c;
                parser >> yr >> c >> mn >> c >> da;
                if (parser.fail()) {
                    ND280Error("Could not parse date: "<< line);
                    continue;
                }
                parser >> hr >> c >> m;
                if (parser.fail()) {
                    ND280Error("Could not parse time: "<< line);
                    continue;
                }
                parser >> std::hex >> hash0;
                if (parser.fail()) {
                    ND280Error("Could not parse hash: "<< line);
                    continue;
                }
                parser >> c >> std::hex >> hash1;
                if (c != '-' || parser.fail()) {
                    ND280Error("Could not parse hash: "<< line);
                    continue;
                }
                parser >> c >> std::hex >> hash2;
                if (c != '-' || parser.fail()) {
                    ND280Error("Could not parse hash: "<< line);
                    continue;
                }
                parser >> c >> std::hex >> hash3;
                if (c != '-' || parser.fail()) {
                    ND280Error("Could not parse hash: "<< line);
                    continue;
                }
                parser >> c >> std::hex >> hash4;
                if (c != '-' || parser.fail()) {
                    ND280Error("Could not parse hash: "<< line);
                    continue;
                }
                yr -= 1900;
                TTimeStamp cTime(yr, mn, da, hr, m, 0 /*s*/, 0 /*ns*/,
                                 true /* Apply shift */,-9*60*60 /*JST*/);
                fHashs.push_back(
                    Hash::value_type(cTime.GetSec(),
                                     ND::TSHAHashValue(hash0,
                                                       hash1,
                                                       hash2,
                                                       hash3,
                                                       hash4)));
            }
            std::sort(fHashs.begin(),fHashs.end(),hashCompare);
            ND280Verbose("Available Geometries:");
            for (Hash::iterator h = fHashs.begin(); h != fHashs.end(); ++h) {
                ND280Verbose("  First valid date: "
                             << TTimeStamp(h->first).AsString("c")
                             << " UTC -- Hash code: "
                             << std::hex << std::setw(8) << std::showbase
                             << h->second);
            }
        }

        // Return the geometry matching the event context.
        virtual ND::TSHAHashValue GetHash(
            const ND::TND280Event* const event) {
            if (fHashs.empty()) return ND::TSHAHashValue();
            // If no event so no default geometry.
            if (!event) return ND::TSHAHashValue();
            // Check to see if there is a hash code for this event.
            ND::TSHAHashValue hc = event->GetGeometryHash();
            if (hc.Valid()) return hc;
            // Find the first context after the event context
            Hash::iterator h;
            for (h = fHashs.begin(); h != fHashs.end(); ++h) {
                if (h->first >= event->GetContext().GetTimeStamp()) break;
            }
            // Before the first context, so use the front.
            if (h == fHashs.begin()) return fHashs.front().second;
            // After  the last context, so use the last.
            else if (h == fHashs.end()) return fHashs.back().second;
            // Use the context before the current.
            return (--h)->second;
        }
    };
    // Create a private geometry lookup.
    DefaultGeometryLookup* gDefaultGeometryLookup = NULL;
}

/// The private constructor for the database
ND::TOADatabase::TOADatabase() {
    fCurrentInputFile = NULL;
    fTPCPadManager = NULL;
    fHATPadManager = NULL;
    fParticleData = NULL;
    fDigits = NULL;
    fGeomId = NULL;
    fGeometryLookup = NULL;
    fAlignmentLookup = NULL;
}

ND::TOADatabase& ND::TOADatabase::Get(void) {
    if (!fOADatabase) {
        ND280Verbose("Create a new ND::TOADatabase object");
        fOADatabase = new ND::TOADatabase;
    }
    return *fOADatabase;
}

TGeoManager* ND::TOADatabase::Geometry(ND::TND280Event* event) {
    // If an event wasn't provided, then try getting the current event.
    if (!event) event = ND::TEventFolder::GetCurrentEvent();
    return GeomId().GetGeometry(event);
}

void ND::TOADatabase::SetCurrentInputFile(TFile* input) {
    fCurrentInputFile = input;
}

void ND::TOADatabase::SetGeometryOverride(const std::string& geomFile) {
    GeomId().SetGeometryFileOverride("");
    GeomId().SetGeometryHashOverride(ND::TSHAHashValue());
    if (geomFile != "") GeomId().SetGeometryFileOverride(geomFile);
}

void ND::TOADatabase::SetGeometryOverride(const ND::TSHAHashValue& hc) {
    GeomId().SetGeometryFileOverride("");
    GeomId().SetGeometryHashOverride(hc);
}

void ND::TOADatabase::RegisterGeometryCallback(
    ND::TOADatabase::GeometryChange* callback) {
    fGeometryCallbacks.insert(callback);
}

void ND::TOADatabase::RemoveGeometryCallback(
    ND::TOADatabase::GeometryChange* callback) {
    fGeometryCallbacks.erase(callback);
}

void ND::TOADatabase::ClearGeometryCallbacks() {
    fGeometryCallbacks.clear();
}

void ND::TOADatabase::ApplyGeometryCallbacks(const ND::TND280Event* event) {
    for (std::set<ND::TOADatabase::GeometryChange*>::iterator
             c = fGeometryCallbacks.begin();
         c != fGeometryCallbacks.end();
         ++c) {
        if (!(*c)) continue;
        (*c)->Callback(event);
    }
}

ND::TOADatabase::GeometryLookup* ND::TOADatabase::RegisterGeometryLookup(
    ND::TOADatabase::GeometryLookup* lookup) {
    ND::TOADatabase::GeometryLookup* old = fGeometryLookup;
    if (!lookup) {
        if (!gDefaultGeometryLookup) {
            gDefaultGeometryLookup = new DefaultGeometryLookup;
        }
        fGeometryLookup = gDefaultGeometryLookup;
    }
    else fGeometryLookup = lookup;
    return old;
}

ND::TSHAHashValue ND::TOADatabase::FindEventGeometry(
    const ND::TND280Event *const event) const {
    TSHAHashValue hashValue;
    if (!fGeometryLookup) {
        if (!gDefaultGeometryLookup) {
            gDefaultGeometryLookup = new DefaultGeometryLookup;
        }
        const_cast<ND::TOADatabase*>(this)->fGeometryLookup
            = gDefaultGeometryLookup;
    }
    try {
        hashValue = fGeometryLookup->GetHash(event);
    }
    catch (...) {
        hashValue = TSHAHashValue();
    }
    return hashValue;
}

ND::TOADatabase::AlignmentLookup* ND::TOADatabase::RegisterAlignmentLookup(
    ND::TOADatabase::AlignmentLookup* lookup) {
    ND::TOADatabase::AlignmentLookup* old = fAlignmentLookup;
    fAlignmentLookup = lookup;
    return old;
}

void ND::TOADatabase::AlignGeometry(const ND::TND280Event* const event) {
    GeomId().ApplyAlignment(event);
}

ND::TAlignmentId ND::TOADatabase::ApplyAlignmentLookup(
    const ND::TND280Event* const event) {
    ND::TAlignmentId id;

    GeomId().GetGeoManager();
    TObjArray* physicalNodes = gGeoManager->GetListOfPhysicalNodes();
    if (physicalNodes) {
        ND280NamedInfo("Geometry","Clear existing physical nodes: "
                       << physicalNodes->GetEntries());
        gGeoManager->ClearPhysicalNodes(true);
    }

    if (fAlignmentLookup) {
        // Save the current geometry state.
        ND280Info("Apply alignment to event");
        gGeoManager->PushPath();

        // Let alignment code know that the alignment is starting.
        id = fAlignmentLookup->StartAlignment(event);

        if (!id.Valid()) ND280NamedInfo("Geometry",
                                        "No alignment should be apply");
        gGeoManager->UnlockGeometry();
        int alignmentCount = 0;
        // Apply alignment to each geometry element.
        TGeometryId geomId;
        while (TGeoMatrix* a = fAlignmentLookup->Align(event,geomId)) {

            // Find the matrix representing the orignal position of
            // geometry object geomID
            std::string path = geomId.GetName();
            if (geomId == TGeometryId()){
                ND280Warn("Geom is empty; skipping");
                continue;
            }
            TGeoPhysicalNode* pNode
                = gGeoManager->MakePhysicalNode(path.c_str());
            TGeoHMatrix* orig = (TGeoHMatrix*) pNode->GetNode()->GetMatrix();
            // As ROOT decided to remove the operators from TGeoMatrix we
            //  have to reproduce the effect of the missing virtual inheritance
            //  here by hand

            //  Find the alignment we need to apply to the geometry object
            TGeoHMatrix h;
            TGeoTranslation* trans= dynamic_cast<TGeoTranslation*> (a);

            if(trans){
                ND280Verbose(" Applying a translation.");
                std::unique_ptr<TGeoTranslation> align(trans);
                h = (*orig) * (*align);
            }
            else
            {
                TGeoCombiTrans* combi=dynamic_cast<TGeoCombiTrans*> (a);
                if(combi){
                    ND280Verbose(" Applying a combination .");
                    std::unique_ptr<TGeoCombiTrans> align(combi);
                    h = (*orig) * (*align);
                }
                else{
                    TGeoRotation* rot=dynamic_cast<TGeoRotation*> (a);
                    if(rot){
                        ND280Verbose(" Applying a rotation.");
                        std::unique_ptr<TGeoRotation> align(rot);
                        h = (*orig) * (*align);
                    }
                    else{
                        ND280Warn(" ND::TOADatabase::AlignGeometry could not cast TGeoMatrix to a real matrix. This should not happen!");
                    }
                }
            }

            if (pNode->IsAligned()) {
                pNode->Align(new TGeoHMatrix(h));
                delete orig;
            }
            else {
                pNode->Align(new TGeoHMatrix(h));
            }
            ++ alignmentCount;
        }

        if (alignmentCount > 0) {
            ND280Info("Applied " << alignmentCount
                      << " alignment matrices.");
        }

        gGeoManager->PopPath();
        gGeoManager->RefreshPhysicalNodes(true);

        if (id.Valid() && !alignmentCount) {
            ND280Error("Invalid alignment applied to geometry");
            throw EBadAlignment();
        }

        if (!id.Valid() && alignmentCount) {
            ND280Error("Invalid alignment applied to geometry");
            throw EBadAlignment();
        }

    }

    // There isn't an alignment id, so create one.  This is the hash of an
    // empty string.
    if (!id.Valid()) {
        ND280NamedDebug("Geometry","No alignment id, so create an empty one");
        SHA1 sha;
        unsigned int message[5];
        if (sha.Result(message)) {
            id = TAlignmentId(message);
        }
        else {
            ND280Error("SHA1 Calculation failed");
        }
    }

    return id;
}

bool ND::TOADatabase::CheckAlignment(const ND::TND280Event* const event) {
    if (!fAlignmentLookup) return false;
    return fAlignmentLookup->CheckAlignment(event);
}

ND::TTPCPadManager& ND::TOADatabase::TPCPads(ND::TND280Event* event) {
    if (!fTPCPadManager) {
        // create the tpc pad information into memory.  The default values
        // almost never change, so they are not saved in the data file.
        // Eventually, there should be a check to read the values from a
        // database.
        fTPCPadManager = new ND::TTPCPadManager();
        if (!fTPCPadManager) {
            throw ENoTPCPads();
        }
    }
    return *fTPCPadManager;
}

ND::THATPadManager& ND::TOADatabase::HATPads(ND::TND280Event* event) {
    if (!fHATPadManager) {
        // create the hat pad information into memory.  The default values
        // almost never change, so they are not saved in the data file.
        // Eventually, there should be a check to read the values from a
        // database.
        fHATPadManager = new ND::THATPadManager();
        if (!fHATPadManager) {
            throw ENoHATPads();
        }
    }
    return *fHATPadManager;
}

TDatabasePDG& ND::TOADatabase::ParticleData(void) {
    if (!fParticleData) {
        fParticleData = TDatabasePDG::Instance();
    }
    return *fParticleData;
}

ND::TPackageSet& ND::TOADatabase::PackageSet(void) {
    return fPackageSet;
}

ND::TDigitManager& ND::TOADatabase::Digits(void) {
    if (!fDigits) {
        fDigits = new ND::TDigitManager();
    }
    return *fDigits;
}

ND::TGeomIdManager& ND::TOADatabase::GeomId(void) {
    if (!fGeomId) {
        fGeomId = new ND::TGeomIdManager();
    }
    return *fGeomId;
}
