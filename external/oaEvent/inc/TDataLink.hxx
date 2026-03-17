// $Id$
//
// The base class for Event Data.
//
#ifndef T_DATALINK_HXX
#define T_DATALINK_HXX

#include <iostream>
#include "TDatum.hxx"

namespace ND {
    OA_EXCEPTION(EBadLink,EDatum);
    class TDataLink;
}

#define TDATALINK_TITLE "TDatum Reference"

/// The TDataLink class provides a way to reference a TDatum object in a TData
/// tree without having the TData become the parent (in other words, a "weak
/// reference").  This is conceptually similar to a hard link in a unix file
/// system.  This prevents THandle loops.
///
/// The TDataLink class should be used when one object in an event wants to
/// reference another.  An example might be when two tracks share some hits,
/// and the reconstruction wants to record the relationship between the
/// tracks.  In this case, both tracks will be owned by a
/// TReconObjectContainer, but each of the tracks needs to have a reference to
/// the other.  This reference would be done using a TDataLink.  For example:
///
/// \code
/// trk1 = ND::THandle<ND::TReconTrack>(new ND::TReconTrack);
/// objectContainer->push_back(trk1);
///
/// trk2 = ND::THandle<ND::TReconTrack>(new ND::TReconTrack);
/// objectContainer->push_back(trk2);
///
/// trk1->AddDatum(new TDataLink("otherTrack",trk2);
/// trk2->AddDatum(new TDataLink("otherTrack",trk1);
/// \endcode
///
/// will mean that the "otherTrack" can be accessed using
/// 
/// \code
/// ND::THandle<ND::TReconTrack> otherTrack 
///                = trk1->Get<ND::TReconTrack>("otherTrack")
/// \endcode
///
/// where this will return a handle to trk2.
class ND::TDataLink : public ND::TDatum {
private:
    /// The datum which this is a link too.
    TDatum *fLink;

protected:
    /// A virtual function that can be used to have "special" Get<> behavior.
    /// This is mostly used by the TDatumLink class so that it can have a weak
    /// reference to TDatum elsewhere in the hierarchy, and have a Get<>
    /// method that refers transparently to the linked object.  The GetThis()
    /// method should be used wherever the link indirection might be desired.
    /// Notice that this will follow the link all the way to the real object
    /// (even if there are several links along the way.
    virtual const TDatum *GetThis(void) const {
        if (!fLink) throw EBadLink();
        return fLink->GetThis();
    }

    virtual TDatum *GetThis(void) {
        if (!fLink) throw EBadLink();
        return fLink->GetThis();
    }

public:
    TDataLink() : TDatum("",TDATALINK_TITLE) { };

    /// Create a new TDataLink.  A TDataLink inherits two fields from TDatum,
    /// "Name" and "Title".  The name is the handle that is used to find
    /// "this" in TData objects.  It can only contain alphanumeric characters
    /// (no "/" or spaces) and *should* be unique with in any level of the
    /// TData hierarchy.  For instance, to get a particular datum out of a
    /// TData object you use data->Get<TDatumClass>("thename").  The title is
    /// a discription of a particular type of datum.  As an example, the MC
    /// interaction data might be named "interaction" and titled "MC
    /// Interaction Data".  The default title is "Event Datum".
    explicit TDataLink(const char* name, ND::THandle<ND::TDatum> link) 
        : TDatum(name,TDATALINK_TITLE) {
        if (!link) return;
        fLink = GetPointer(link);
    }

    virtual ~TDataLink();

    /// Set the TDatum that is being linked.
    virtual void SetLink(ND::TDatum *link) {fLink = link;}

    /// Set the TDatum that is being linked.
    virtual void SetLink(ND::THandle<ND::TDatum> link) {
        fLink = GetPointer(link);
    }

    /// Print the datum information.
    virtual void ls(Option_t* opt = "") const {
        TDatum::ls(opt);
        TROOT::IncreaseDirLevel();
        TROOT::IndentLevel();
        std::cout << "-> " << fLink->GetThis()->ClassName() 
             << "(" << fLink->GetThis() << ")::" 
             << "Name = " << fLink->GetThis()->GetFullName();
        if (strstr(opt,"title")) {
            std::cout << " Title = " << fLink->GetThis()->GetTitle();
        }
        std::cout << std::endl;
        TROOT::DecreaseDirLevel();
    };

    /// Check if this link is to a browsable class.
    virtual Bool_t IsFolder(void) const;

    /// Call the browser for the linked class.
    virtual void Browse(TBrowser* b);

protected:
    ClassDef(TDataLink,1);
};
#endif
