// $Id$
//
// The base class for Event Data.
//
#ifndef T_DATA_SYM_LINK_HXX
#define T_DATA_SYM_LINK_HXX

#include <iostream>

class TBrowser;

#include "TDatum.hxx"

namespace ND {
    OA_EXCEPTION(EBadSymLink,EDatum);
    class TDataSymLink;
}

#define TDATA_SYM_LINK_TITLE "Link To"

/// The TDataSymLink class provides a way to reference a TDatum object in a
/// TData tree without having the TData object become the parent.  This is
/// conceptually similar to a symbolic link in a unix file system.
class ND::TDataSymLink : public TDatum {
private:
    /// The datum which this is a link too.
    TString fLink;

public:
    /// A virtual function that can be used to have "special" Get<>
    /// behavior.  This is mostly used by the TDatumLink class so that
    /// it can have a weak reference to TDatum elsewhere in the
    /// hierarchy, and have a Get<> method that refers transparently to
    /// the linked object.  The GetThis() method should be used
    /// wherever the link indirection might be desired.  Notice that
    /// this will follow the link all the way to the real object (even
    /// if there are several links along the way.
    virtual const TDatum *GetThis(void) const {
        THandle<TDatum> parent = GetParent<TDatum>();
        if (!parent) return NULL;
        // The name in fLink relative to the parent.
        THandle<TDatum> link = parent->Get<TDatum>(fLink);
        if (!link) return NULL;
        return link->GetThis();
    };

    virtual TDatum *GetThis(void) {
        THandle<TDatum> parent = GetParent<TDatum>();
        if (!parent) return NULL;
        // The name in fLink relative to the parent.
        THandle<TDatum> link = parent->Get<TDatum>(fLink);
        if (!link) return NULL;
        return link->GetThis();
    };

public:
    TDataSymLink() : TDatum("",TDATA_SYM_LINK_TITLE) { };

    /// Create a new TDataSymLink.  A TDataSymLink inherits two fields from
    /// TDatum, "Name" and "Title".  The name is the handle that is
    /// used to find "this" in TData objects.  It can only contain
    /// alphanumeric characters (no "/" or spaces) and///should* be
    /// unique with in any level of the TData hierarchy.  For instance,
    /// to get a particular datum out of a TData object you use
    /// data->Get<TDatumClass>("thename").  The title is a discription
    /// of a particular type of datum.  As an example, the MC
    /// interaction data might be named "interaction" and titled "MC
    /// Interaction Data".  The default title is "Event Datum".
    explicit TDataSymLink(const char* name, const char *link = NULL) 
        : TDatum(name,TDATA_SYM_LINK_TITLE) {
        SetLink(link);
    };

    virtual ~TDataSymLink();

    /// Set the TDatum that is being linked.
    virtual void SetLink(const char* link) {
        if (link) fLink = link;
        else fLink = "";
    };

    /// Print the datum information.
    virtual void ls(Option_t* opt = "") const;

    /// Check if this link is to a browsable class.
    virtual Bool_t IsFolder(void) const;

    /// Call the browser for the linked class.
    virtual void Browse(TBrowser* b);

private:
    TDatum* RemoveDatum(TDatum*){
        MayNotUse("RemoveDatum");
        return NULL;
    }
    void InsertDatum(TDatum*){
        MayNotUse("InsertDatum");
    }
    TDatum* FindDatum(const char *){
        MayNotUse("FindDatum");
        return NULL;
    }

protected:
    ClassDef(TDataSymLink,2);
};
#endif
