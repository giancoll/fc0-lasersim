#ifndef TOADATABASE_HXX_SEEN
#define TOADATABASE_HXX_SEEN

#include <iostream>
#include <map>
#include <set>

#include <TObject.h>

#include "EoaCore.hxx"
#include "TPackageSet.hxx"
#include "TSHAHashValue.hxx"
#include "TAlignmentId.hxx"

class TGeoManager;
class TDatabasePDG;
class TFile;
class TTree;
class TGeoMatrix;

namespace ND {
    class TTPCPadManager;
    class THATPadManager;
    class TND280Event;
    class TOADatabase;
    class TDigitManager;
    class TGeomIdManager;
    class TND280Input;
    class TGeometryId;

    /// An exception from the data base.
    OA_EXCEPTION(EOADatabase,EoaCore);

    /// Flag that no file was found.
    OA_EXCEPTION(ENoInputFile,EOADatabase);

    /// Flag that geometry data base was not found.
    OA_EXCEPTION(ENoGeometry,EOADatabase);

    /// Flag that something went wrong with the alignment.
    OA_EXCEPTION(EBadAlignment, ENoGeometry);

    /// Flag that tpc pad information data base was not found.
    OA_EXCEPTION(ENoTPCPads,EOADatabase);

    /// Flag that tpc pad information data base was not found.
    OA_EXCEPTION(ENoHATPads,EOADatabase);
}

/// Static access to the various geometry and calibration databases.  The main
/// table provided by this class are the ROOT geometry using
/// ND::TOADatabase::Get().Geometry().  It also provides several managers such
/// as Digits(), GeomId(), HATPads, TPCPads, and the PDG particle table in Particles().
///
/// This class is mostly used to access the root geometry using the Geometry()
/// method.  This method takes an event pointer as an argument, or uses the
/// current event in memory, and verifies that the correct geometry is
/// available.  In addition, if the oaApplyAlign library is available, it will
/// apply the alignment constant to the current event.
///
/// If user code needs to fill tables with geometry information (e.g. the
/// internal RECPACK sensor geometry), a ND::TOADatabase::GeometryChange call
/// back should be registered using RegisterGeometryCallback().  See the
/// ND::TOADatabase::GeometryChange class documentation for more details.
///
class ND::TOADatabase {
    friend class ND::TND280Input;
    friend class ND::TGeomIdManager;

public:

    /// A base class to be registered by users providing a callback whenever
    /// the geometry has changed.  The callback is registered using the
    /// ND::TOADatabase::RegisterGeometryCallback() method.
    ///
    /// This class is most usefule whenever the a user has tables that need to
    /// be reinitialized after a geometry change.  For example, if a user
    /// builds a table of sub-detector masses.  To use this class, a user will
    /// declare a local class derived from GeometryChange that overrides the
    /// Callback method.
    ///
    /// \code
    /// class MyChangeClass: public ND::TOADatabase::GeometryChange {
    /// public:
    ///    void Callback(const ND::TND280Event* const event) {
    ///        // Do something with the current geometry
    ///        TGeoManager* newGeom = ND::TOADatabase::Get().Geometry();
    ///    }
    /// };
    /// \endcode
    ///
    /// This callback must then be registered with TOADatabase.
    ///
    /// \code
    /// MyChangeClass* myChangeClass = new MyChangeClass;
    /// ND::TOADatabase::Get().RegisterGeometryCallback(myChangeClass);
    /// \endcode
    ///
    /// All of the callbacks registered with TOADatabase will then be called
    /// when the geometry changes.  The order of callbacks is unspecified
    /// (meaning don't count on a particular call order).
    class GeometryChange {
    public:
        GeometryChange() {}
        virtual ~GeometryChange() {}

        // A method to be implemented by the user that will be called each
        // time the geometry has changed.  The call back is called after the
        // alignment has been applied.  This method should not modify the
        // event.  In addition, don't depend on the order in which the
        // registered callbacks will be executed.
        virtual void Callback(const ND::TND280Event* const event) = 0;
    };

    /// A base class to be registered to find the geometry associated with an
    /// event.  In general, a derived class will be implemented in a separate
    /// package and use off-line calibration database to associate an event
    /// context with a particular geometry.  The derived class must implement
    /// the GetHash method.
    class GeometryLookup {
    public:
        GeometryLookup() {}
        virtual ~GeometryLookup() {}
        
        /// Return the hash value of the geometry that should be used with the
        /// event.  If the hash value cannot be determined, an invalid hash
        /// should be returned.  If an invalid hash is returned, then a
        /// default geometry file will be used.
        ///
        /// Note: The hash value is the last method of finding a geometry file
        /// and can be overridden by providing a geometry directly in the data
        /// file, setting the geometry file explicitly using
        /// ND::TOADatabase::SetGeometryFile(), or setting the hash value
        /// explicitly using ND::TOADatabase::SetGeometryHash().
        /// 
        /// This class is used in the FindEventGeometry method.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-parameter"
        virtual TSHAHashValue GetHash(const ND::TND280Event* const event) {
            return TSHAHashValue();
        }
    #pragma GCC diagnostic pop

    };

    /// A base class to be registered to find the alignment parameters
    /// associated with a particular geometry.  In general, a derived class
    /// will be implemented in a separate package and use off-line calibration
    /// database to associate an event context with a particular alignment.
    ///
    /// The derived class must implement the Align method.  The StartAlignment
    /// methods can be optionally implemented and is used to notify the
    /// AlignmentLookup class that a new alignment is being started.
    /// 
    /// This class is used in the AlignGeometry method.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    class AlignmentLookup {
    public:
        AlignmentLookup() {}
        virtual ~AlignmentLookup() {}

        /// Called to find out if the alignment should be reapplied.  If the
        /// alignment should be reapplied this will return true.  If the
        /// geometry has been changed, then the alignment will be reapplied
        /// regardless of the CheckAlignment return value.
        virtual bool CheckAlignment(const ND::TND280Event* const event) {
            return true; 

        }
        
        /// Called before the geometry alignment for an event is begun.  This
        /// is used to notify the implemented class that a new alignment is
        /// about to start.  It will generally trigger initialization.  If an
        /// alignment should not be applied, then this should return an empty
        /// alignment.
     
        virtual ND::TAlignmentId 
        StartAlignment(const ND::TND280Event* const event) {
            return ND::TAlignmentId();
        }

        
        /// Return the alignment for a particular geometry id.  This will be
        /// called multiple times.  The alignment correction is returned as a
        /// TGeoMatrix pointer.  When there is no more alignment to be
        /// returned, this should return a NULL TGeoMatrix pointer.  The
        /// geomId argument is used to return the geometry identifier of the
        /// volume to be aligned.  The ownership of the TGeoMatrix is claimed
        /// by the calling routine, so it must be generated using "new".
        ///
        /// The returned alignment is relative to the nominal position and
        /// rotation of the volume, and is specified in the master coordinate
        /// system.  
        virtual TGeoMatrix* Align(const ND::TND280Event* const event,
                                  TGeometryId& geomId) {return NULL;}

    };
    #pragma GCC diagnostic pop

    virtual ~TOADatabase() {;};

    /// Get a pointer to the singleton instance of the database.
    static TOADatabase& Get(void);

    /// Return a pointer to the correct detector geometry for the event.  If
    /// the argument is NULL, then this will try to get the geometry for the
    /// current event (as available from TEventFolder::GetCurrentEvent(), or
    /// the most recent available geometry.  This method leaves the geometry
    /// unchanged if the current geometry is valid, and the current event
    /// context has not changed.
    ///
    /// \note This method may change the state of the geometry.  This cannot
    /// be changed since the geometry may or may not needed to be reloaded
    /// depending on the state of the event.  Users should plan on either
    /// checking the state of the geometry after using this method, or setting
    /// the geometry state using CdNode, or FindNode.  In addition, users
    /// should assume that calling this method could invalidate any node
    /// identifiers they may be using. (e.g. the result of
    /// TGeoManager::GetCurrentNodeId()).
    ///
    /// The geometry is searched for in the following order: If the geometry
    /// file has been explicity set using ND::TOADatabase::SetGeometryFile(),
    /// then use that geometry.  Otherwise, if an explicit geometry hash value
    /// has been set using ND::TOADatabase::SetGeometryHash(), use that
    /// geometry.  Otherwise, if the event file contains a geometry, try to
    /// use that geometry.  Otherwise, use the geometry associated with a hash
    /// value returned by the registered ND::TOADatabase::GeometryLookup class
    /// which generally looks up the hash value using the calibration
    /// database.
    /// 
    /// \note The new geometry will only be loaded if there is a current
    /// event.  If there isn't a current event, a previously loaded geometry
    /// will be returned.  If there isn't a current and there isn't a
    /// previously loaded geometry, this will throw a ENoGeometry exception.
    /// If you want to force a geometry to be loaded even though you don't
    /// currently have access to an event, you can use
    /// ND::TOADatabase::Get().GeomId().ReadEvent(ND::TSHAHashValue()).  While
    /// this isn't convenient, loading a geometry without an event is also
    /// rather unusual.
    /// 
    /// If none of the attempts to find a geometry work this throws an
    /// ENoGeometry exception; however, since not having a geometry is a very
    /// serious error you will probably want your program to crash so you
    /// don't need to catch the ENoGeometry exception.
    ///
    /// After the geometry is loaded alignment parameters are applied.  The
    /// alignment is applied using the AlignGeometry() method which uses
    /// ND::TOADatabase::AlignmentLookup class to lookup the coeficients.
    ///
    /// Users should register the ND::TOADatabase::GeometryLookup and
    /// ND::TOADatabase::AlignmentLookup classes using the
    /// RegisterGeometryLookup and RegisterAlignmentLookup methods.
    ///
    /// If you need to use tables that get initialized for each new geometry
    /// (for instance, you might want to cache the locations of various
    /// detectors for use in RECPACK), then you need to define and register a
    /// ND::TOADatabase::GeometryChange class using
    /// ND::TOADatabase::RegisterGeometryCallback.  See the
    /// ND::TOADatabase::GeometryChange for details and an example.
    TGeoManager* Geometry(TND280Event* event=NULL);

    /// Return a pointer to the correct TPC pad information for the event.  If
    /// the argument is NULL, the pad information from the current event (as
    /// determined by TEventFolder::GetCurrentEvent()) is used, or failing
    /// that, the most recent pad information is used.  If none of the
    /// attempts to find the pad information work this throws an ENoTPCPads
    /// exception; however, since not having pad information is a very serious
    /// error you will probably want your program to crash so you don't need
    /// to catch the ENoTPCPads exception.
    TTPCPadManager& TPCPads(TND280Event* event=NULL);

    /// Return a pointer to the correct HAT pad information for the event.  If
    /// the argument is NULL, the pad information from the current event (as
    /// determined by TEventFolder::GetCurrentEvent()) is used, or failing
    /// that, the most recent pad information is used.  If none of the
    /// attempts to find the pad information work this throws an ENoHATPads
    /// exception; however, since not having pad information is a very serious
    /// error you will probably want your program to crash so you don't need
    /// to catch the ENoHATPads exception.
    THATPadManager& HATPads(TND280Event* event=NULL);

    /// Return a reference to the ROOT TDatabasePDG.
    TDatabasePDG& ParticleData(void);

    /// Return a reference to the set of packages linked to the current
    /// executable.
    TPackageSet& PackageSet(void);

    /// Return a reference to the TDigitManager.
    TDigitManager& Digits(void);

    /// Return a reference to the TGeomIdManager.  The TGeomIdManager class
    /// manages the geometries and alignments that are loaded to use with an
    /// event.  It provides the implementation required to implement
    /// TOADatabase::Geometry().  All book keeping information about the
    /// current geometry (such as it's hashcode or the alignment id) should be
    /// accessed through TGeomIdManager.
    TGeomIdManager& GeomId(void);

    /// @{ Provide a file to override the standard geometry.  Setting the
    /// geometry file to any value other than "" will cause TOADatabase to
    /// override the standard (default) geometry with the one found in the
    /// geometry file.  You can generate a geometry file using the ND280GEANT4SIM.exe
    /// /t2k/export macro command.  This resets the required geometry if
    /// called without an argument.
    void SetGeometryOverride(const std::string& geoName ="");
    void SetGeometryOverride(const ND::TSHAHashValue& hc);
    /// @}

    /// Register a user callback that will be called whenever the geometry
    /// changes (see the ND::TOADatabase::GeometryChange class).
    void RegisterGeometryCallback(ND::TOADatabase::GeometryChange* callback);

    /// Remove a user callback.
    void RemoveGeometryCallback(ND::TOADatabase::GeometryChange* callback);

    /// Remove all user callbacks.
    void ClearGeometryCallbacks();

    /// Register a class implementing the search for the correct geometry to
    /// be used for an event.  If called with "NULL", the default geometry
    /// lookup is installed.
    ND::TOADatabase::GeometryLookup* RegisterGeometryLookup(
        ND::TOADatabase::GeometryLookup* lookup);

    /// Register a class implementing the search for the correct geometry to
    /// be used for an event.  A pointer to the previous alignment lookup
    /// class is returned.
    ND::TOADatabase::AlignmentLookup* RegisterAlignmentLookup(
        ND::TOADatabase::AlignmentLookup* lookup);

    /// Apply the alignment to the current geometry.  The alignment is usually
    /// done automatically when the geometry is loaded, but this can be called
    /// multiple times by the user as well.  Each time it is called, the
    /// current set of physical nodes is cleared and the alignment is
    /// reapplied to the baseline geometry.  
    ///
    /// Be careful, the ROOT physical geometry alignment code is a relatively
    /// slow.  In practical terms, the alignment should only be applied when
    /// the geometry is loaded.  
    ///
    /// If this applies an alignment, it will return the TAlignmentId
    /// associated with the new alignment.  Otherwise, it will return an
    /// invalid alignment.
    virtual void AlignGeometry(const ND::TND280Event* const event);

private:
    TOADatabase();
    TOADatabase(const TOADatabase&);
    TOADatabase& operator=(const TOADatabase&);

    /// Find the hash code for a particular event.  This uses the
    /// GeometryLookup class to lookup the correct hash code for the event.
    /// If the event hash code is not found, then this returns an invalid hash
    /// code.  This is used by TGeomIdManager to access the GeometryLookup
    /// class.
    TSHAHashValue FindEventGeometry(const TND280Event *const event) const;

    /// Call the registered geometry callbacks.  This is used by
    /// TGeomIdManager to notify the user routines when the geometry changes.
    void ApplyGeometryCallbacks(const ND::TND280Event* event);

    /// Use the AlignmentLookup method to check if a new alignment should be
    /// applied.  If this returns true, the we need a new alignment.  This is
    /// used by TGeomIdManager to short circuit the alignment application.
    bool CheckAlignment(const ND::TND280Event* const event);

    /// Use the AlignmentLookup class to get the actual alignment.
    virtual ND::TAlignmentId ApplyAlignmentLookup(const ND::TND280Event* event);

    /// Set the current input file for the data base.  This must be set for
    /// many of the methods to work correctly.  This is used by TND280Input to
    /// notify TOADatabase when the input file changes.
    void SetCurrentInputFile(TFile* input);
    
    /// Get the current input event file.  This is used by TGeomIdManager to
    /// find the file with the geometry.
    TFile* CurrentInputFile(void) {return fCurrentInputFile;}

    static TOADatabase *fOADatabase;
    
    /// The current event file.  This is only valid for a ROOT input file.
    TFile* fCurrentInputFile;

    /// A pointer to a class which will find the correct geometry to be used
    /// with an event.
    GeometryLookup* fGeometryLookup;

    /// A pointer to a class which will find the alignment associated with the
    /// current geometry used for this event.
    AlignmentLookup* fAlignmentLookup;

    /// The current TPC pad manager.  This is fixed for all events.
    TTPCPadManager* fTPCPadManager;

    /// The current HAT pad manager.  This is fixed for all events.
    THATPadManager* fHATPadManager;

    /// A connection to the ROOT particle data book data base.
    TDatabasePDG* fParticleData;

    /// The set of packages linked with the executable.
    TPackageSet fPackageSet;
    
    /// Manage the connection to the oaRawEvent digitized data.
    TDigitManager* fDigits;

    /// Manage the mapping of the root TGeoManager node identifiers to the
    /// geometry identifiers.
    TGeomIdManager* fGeomId;

    /// A vector of call backs that will be called whenever the geometry
    /// changes.
    std::set<ND::TOADatabase::GeometryChange*> fGeometryCallbacks;

    ClassDef(TOADatabase,0);
};
#endif
