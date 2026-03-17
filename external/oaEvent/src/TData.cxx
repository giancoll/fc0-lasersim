// $Id$
//
// Implement the ND::TData class.  This is a very simple class so almost
// all of the methods are implemented in line.  However the ClassImp
// macro must be in a separate file and the virtual distructor is also
// implemented here.

#include "TData.hxx"

// The default destructor.
ND::TData::~TData() { }

void ND::TData::AddDatum(ND::TDatum* val, const char* name) {
    if (!val) {
        ND280Severe("Datum pointer has null value");
        throw EBadInsertion();
    }
    if (name) val->SetName(name);
    val->SetBit(kCanDelete,false);
    InsertDatum(val);
}

void ND::TData::AddDatum(ND::THandle<ND::TDatum> handle, const char* name) {
    // Make sure the handle is valid;
    if (!handle) throw EBadInsertion();
    // Make sure that handle isn't holding an object that already belongs
    // in the event.
    if (handle->GetParentDatum()) {
        ND280Severe("Datum appears to have already been inserted");
        throw EBadInsertion();
    }
    handle.Release();
    TDatum* val = GetPointer(handle);
    AddDatum(val,name);
}

// The default RemoveDatum.  This always aborts since it must have
// been implemented by the derived class.
ND::TDatum* ND::TData::RemoveDatum(ND::TDatum*) {
    AbstractMethod("TData::RemoveDatum");
    return NULL;
}

// The default FindDatum never finds the object since there is never
// an object to find.
ND::TDatum* ND::TData::FindDatum(const char*) {
    AbstractMethod("TData::FindDatum");
    return NULL;
}

// The default InsertDatum cannot really add data since there is no
// container in the base ND::TData.
void ND::TData::InsertDatum(ND::TDatum *) {
    AbstractMethod("TData::InsertDatum");
}

