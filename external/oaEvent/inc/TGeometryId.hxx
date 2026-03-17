#ifndef TGeometryId_hxx_seen
#define TGeometryId_hxx_seen

#include <Rtypes.h>
#include <TVector3.h>
#include <string>

#include "EoaCore.hxx"
#include "method_deprecated.hxx"

namespace ND {
    /// Base class for all exceptions associated with the TGeometryId classe.
    OA_EXCEPTION(EGeomId,EoaCore);

    /// Invalid MSB or LSB.
    OA_EXCEPTION(EGeomIdMSBLSB,EGeomId);

    /// Invalid geometry id.
    OA_EXCEPTION(EGeomIdInvalid,EGeomId);

    /// Value out of range.
    OA_EXCEPTION(EGeomIdOutOfRange,EGeomIdInvalid);

    class TGeometryId;
}

/// Geometry identifier class to uniquely identify a particular volume within
/// the detector.  This is an opaque type used to index the databases.  It
/// should be queried using the GetName() method which returns the volume path
/// within the current detector geometry.  The TGeomIdManager and the
/// functions in the ND::GeomId namespace provide additional ways to query the
/// identifier.
///
/// Examples:
///
/// To set the id to a P0D P0D scintillator bar:
/// \code
/// TGeometryId id = ND::GeomId::P0D::Bar(p0duleNumber, xyLayer, bar);
/// \endcode
///
/// To set the id to a TPC pad:
/// \code
/// TGeometryId id = ND::GeomId::TPC::Pad(tpc, half, mm, pad);
/// \endcode
///
/// Find the detector element:
/// \code
/// std::string name = geomId.GetName();
/// if (name.find("P0Dule") != std::string::npos) {/* In P0Dule */}
/// \endcode
///
/// Find a element number (e.g. a MicroMega):
/// \code
/// std::string name = geomId.GetName();
/// std::size_type mm = name.find("/MM_");
/// if (mm != std::string::npos) {
///     std::istringstream in(name.substr(mm+4);
///     in >> microMegaNumber;
/// }
/// \endcode
///
/// The ROOT geometry node can be found:
/// \code
/// TOADatabase::Get().GeomId().CdId(geomId);
/// int nodeId = gGeoManager->GetCurrentNodeId();
/// \endcode
///
/// The geometry identifier for a particular position.
/// \code
/// // Return false if x,y,z is an invalid position.
/// TOADatabase::Get().GeomId().GetGeometryId(x,y,z,geomId);
/// \endcode
///
class ND::TGeometryId {
public:
    TGeometryId();
    TGeometryId(const TGeometryId& geom);
    explicit TGeometryId(int id);
    virtual ~TGeometryId();

    /// Get the internal integer representation
    int AsInt() const {return fGeometryId;}

    /// Check that this is a valid geometry id.  This method checks that the
    /// geometry id is correctly formed, and not that it corresponds to a real
    /// detector component.
    bool IsValid() const;

    /// Get the geometry path name for this id.  This method should be used to
    /// determine which detector element is represented by the identifier.
    std::string GetName() const;

    /// Get the subsystem name.  The current values are "P0D", "TPC", "HAT",
    /// "FGD", "DSECal", "TECal", "PECal", "SMRD", "INGRID", "SFG", "TOF" and
    /// "node".  If the value is empty ("node"), then the identifier is a
    /// plain ROOT node identifier, and you must use the geometry manager to
    /// query for the specific location.
    std::string GetSubsystemName() const;

    /// Get the subsystem index.  The values are defined in ND280GeomIdDef.hxx
    /// and are
    ///
    ///    * ND::GeomId::Def::kP0D           -- The P0D
    ///    * ND::GeomId::Def::kHAT           -- The HAT
    ///    * ND::GeomId::Def::kTPC           -- The TPC
    ///    * ND::GeomId::Def::kFGD           -- The FGD
    ///    * ND::GeomId::Def::kDSECal        -- The DSECal
    ///    * ND::GeomId::Def::kTECal         -- The TECal
    ///    * ND::GeomId::Def::kPECal         -- The PECal
    ///    * ND::GeomId::Def::kSMRD          -- The SMRD
    ///    * ND::GeomId::Def::kTOF          -- The TOF
    ///    * ND::GeomId::Def::kINGRID        -- The INGRID
    ///    * ND::GeomId::Def::kSFG           -- The SuperFGD
    ///    * ND::GeomId::Def::kTOF           -- The Time Of Flight
    ///    * ND::GeomId::Def::kROOTGeoNodeId -- A general ROOT node
    ///
    /// Any other value is invalid.
    int GetSubsystemId() const;

    /// Get the volume position for this geometry identifier.  This is
    /// provided as a convenient interface to the
    /// TGeomIdManager::GetPosition() method.  The TGeomIdManager method
    /// provides return values that flag if the position is not available, and
    /// it should be preferred.
    TVector3 GetPosition() const;

    bool operator <(const ND::TGeometryId& rhs) const {
        return AsInt() < rhs.AsInt();
    }

    /// DO NOT USE.  This method is deprecated and will be removed in a future
    /// release.  Use the functions provided in the ND280GeomId ND::GeomId
    /// namespace to set the TGeometryId fields.
    void SetField(int val, int msb, int lsb) METHOD_DEPRECATED
        {SetFieldSafe(val,msb,lsb);}

    /// DO NOT USE.  This method is deprecated and will be removed in a future
    /// release.  Use the functions provided in the ND280GeomId ND::GeomId
    /// namespace to get the TGeometryId fields.
    int GetField(int msb, int lsb) const METHOD_DEPRECATED
        {return GetFieldSafe(msb,lsb);}

protected:
    /// Set the field in a bit range. The bit range is between 0
    /// (lsb) and 31 (msb).  Note that to set a single bit, the lsb will equal
    /// the msb.  This class is reserved for internal implementation.
    void SetFieldSafe(int val, int msb, int lsb);

    /// Get a field from a bit range.  This class is reserved for
    /// internal implementation.
    int GetFieldSafe(int msb, int lsb) const;

    int fGeometryId;
    ClassDef(TGeometryId, 1)
};

bool operator ==(const ND::TGeometryId& a, const ND::TGeometryId& b);
bool operator !=(const ND::TGeometryId& a, const ND::TGeometryId& b);
std::ostream& operator<<(std::ostream& s, const ND::TGeometryId& id);
#endif
