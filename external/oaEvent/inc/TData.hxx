// $Id$
//
// A base class for event data containers.
//
#ifndef T_DATA_HXX
#define T_DATA_HXX

#include "EoaCore.hxx"
#include "TDatum.hxx"
#include "THandle.hxx"

namespace ND {
    OA_EXCEPTION(EData,EDatum);
    OA_EXCEPTION(EBadInsertion,EData);
    class TData;
}

#define TDATA_TITLE "Event Data"
/// The abstract base container class for data stored in a named hierarchy.
/// This is similar to a TFolder, but the elements of the hierarchy keep track
/// of the object which owns them (there is a back pointer).  This class is
/// inherited by classes which implement specific container behavior.  The
/// TDataVector class behaves like a vector or dictionary (and is the most
/// commonly used type).
class ND::TData : public TDatum {
public:
    TData() : TDatum("",TDATA_TITLE) { };

    /// Create a new TData.  A TData inherits two fields from TNamed,
    /// "Name" and "Title".  The name is the handle that is used to
    /// find "this" in TData objects.  It can only contain alphanumeric
    /// characters (no "/" or spaces) and *should* be unique with in
    /// any level of the TData hierarchy.  If the name is not unique,
    /// then only the first object with that name will be accessible
    /// using the Get method.  For instance, to get a particular datum
    /// out of a TData object you use data->Get<TDataClass>("thename").
    /// The title is a discription of a particular type of data.  As an
    /// example, the MC interaction data might be named "interaction"
    /// and titled "MC Interaction Data".  The default title is "Event Data". 
    explicit TData(const char* name, const char* title = TDATA_TITLE) 
        : TDatum(name,title) { };

    virtual ~TData();

    /// A default way to add a TDatum.  Derived classes will usually
    /// provide a specialized insertion method that is optimized for
    /// the class, but this provides a default interface that can be
    /// used in most circumstances.
    virtual void AddDatum(TDatum* val, const char* name=NULL);

    /// A default way to add a THandle<TDatum>.  This takes over memory
    /// management of the handle and will throw the EBadInsertion
    /// exception if the insertion fails.
    virtual void AddDatum(ND::THandle<ND::TDatum> handle,
                          const char* name=NULL);

    virtual TDatum* FindDatum(const char* name);

    /// This method finds the element and if the element is in the parent, it
    /// is removed.  This method temporarily takes ownership of the element,
    /// and then returns the element as the method return value.  The derived
    /// classes must implement this method to actually remove the element.
    /// See documentation for ND::TDatum::RemoveDatum() for details.
    virtual TDatum* RemoveDatum(TDatum* element) = 0;

protected:
    /// A virtual method that is used internally to help move the data (It
    /// should only be used in TData).  This method must insert the element
    /// into the container and set the parent.
    virtual void InsertDatum(TDatum *element) = 0;

    ClassDef(TData,3);
};
#endif
