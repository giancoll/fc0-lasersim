// $Id$
//
// Implement the ND::TDataVector class.  This is a very simple class so almost
// all of the methods are implemented in line.  However the ClassImp
// macro must be in a separate file and the virtual distructor is also
// implemented here.
#include <algorithm>

#include <TBrowser.h>

#include "TDataVector.hxx"

ClassImp(ND::TDataVector);

// The default destructor.
ND::TDataVector::~TDataVector() {
    Clear();
}

ND::TDataVector::iterator ND::TDataVector::begin(void) {
    return iterator(this,fVector.begin());
}

ND::TDataVector::iterator ND::TDataVector::end(void) {
    return iterator(this,fTemporary.end());
}

ND::TDataVector::const_iterator ND::TDataVector::begin(void) const {
    return const_iterator(this,fVector.begin());
}

ND::TDataVector::const_iterator ND::TDataVector::end(void) const {
    return const_iterator(this,fTemporary.end());
}

ND::TDataVector::reverse_iterator ND::TDataVector::rbegin(void) {
    return reverse_iterator(end());
}

ND::TDataVector::reverse_iterator ND::TDataVector::rend(void) {
    return reverse_iterator(begin());
}

ND::TDataVector::const_reverse_iterator ND::TDataVector::rbegin(void) const {
    return const_reverse_iterator(end());
}

ND::TDataVector::const_reverse_iterator ND::TDataVector::rend(void) const {
    return const_reverse_iterator(begin());
}

ND::TDataVector::iterator ND::TDataVector::erase(TDatum* element) {
    if (element) {
        // Protect against trying to removing an element that not part of this
        // TDataVector.
        if (element->GetParentDatum() != this) throw ENoSuchElement();
        element->AssignParentDatum(NULL);
    }
 
    // Now make sure that the element is removed from this objects storage.
    TDatumVector::iterator i = std::find(fVector.begin(),fVector.end(),element);
    if (i != fVector.end()) return iterator(this,fVector.erase(i));

    i = std::find(fTemporary.begin(),fTemporary.end(),element);
    return iterator(this,fTemporary.erase(i));
}    

ND::TDataVector::iterator ND::TDataVector::erase(ND::THandle<ND::TDatum> e) {
    return erase(ND::GetPointer(e));
}

ND::TDataVector::iterator ND::TDataVector::erase(iterator position) {
    // Protect against passing an iterator to the end. (A common mistake).
    if (position == end()) return end();

    return erase(*position);
}    

ND::TDataVector::iterator ND::TDataVector::insert(iterator position, 
                                                  TDatum* element) {
    if (!element) return end();
    element->SetBit(kCanDelete,false);

    // Check if "position" is part of fVector, and insert the element at the
    // end of the permenant objects if it's not.
    ND::TDatumVector::iterator i
        = std::find(fVector.begin(),fVector.end(),*position);
    return iterator(this,fVector.insert(i,element));
}

void ND::TDataVector::push_back(TDatum* element) {
    AddDatum(element);
}

// Find a TDatum in the data vector. 
ND::TDatum* ND::TDataVector::FindDatum(const char* name) {
    TDatumVector::reverse_iterator i;
    
    // Check to see if the object is in the temporary storage.
    i = std::find_if(fTemporary.rbegin(),fTemporary.rend(),
                ND::TDatumCompareName(name));
    if (i != fTemporary.rend()) return (*i);

    // Check to see if the object is in the main storage.
    i = std::find_if(fVector.rbegin(),fVector.rend(),
                     ND::TDatumCompareName(name));
    if (i != fVector.rend()) return (*i);
    
    return NULL;
}

// Remove the element from this TDataVector object.
ND::TDatum* ND::TDataVector::RemoveDatum(ND::TDatum* element) {
    ND::TDataVector::iterator i = std::find(begin(),end(), element);
    if (i == end()) return NULL;
    erase(i);
    return element;
}

// Insert the element into the TDataVector object.
void ND::TDataVector::InsertDatum(ND::TDatum* val) {
    if (!val) return;
    val->ReassignParentDatum(this);
    fVector.push_back(val);
}

// Insert the element into the temporary storage. 
void ND::TDataVector::InsertTemporary(ND::TDatum* val) {
    if (!val) return;
    val->ReassignParentDatum(this);
    fTemporary.push_back(val);
}

// Add a temporary datum to the TDataVector object.  The temporary object will
// be saved in fTemporary.
void ND::TDataVector::AddTemporary(ND::TDatum* val, const char* name) {
    if (!val) {
        ND280Severe("Datum pointer has null value");
        throw EBadInsertion();
    }
    if (name) val->SetName(name);
    val->SetBit(kCanDelete,false);
    InsertTemporary(val);
}

// Add a temporary datum to the TDataVector object.  The temporary object will
// be saved in fTemporary.
void ND::TDataVector::AddTemporary(ND::THandle<ND::TDatum> handle,
                                   const char* name) {
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
    AddTemporary(val,name);
}

bool ND::TDataVector::IsTemporary(ND::TDatum* val) {
    return (fTemporary.end() 
            != std::find(fTemporary.begin(), fTemporary.end(),val));
}

bool ND::TDataVector::IsTemporary(ND::THandle<ND::TDatum> val) {
    return (fTemporary.end() 
            != std::find(fTemporary.begin(), fTemporary.end(),
                         ND::GetPointer(val)));
}

// Delete the contents of the ND::TDataVector.
void ND::TDataVector::Clear(Option_t*) {
    for (iterator i = begin(); i != end(); ++i) {
        (*i)->AssignParentDatum(NULL);
        delete (*i);
    }
}

// Add all of the contents to the browser.
void ND::TDataVector::Browse(TBrowser *b) {
    if (b) {
        for (iterator i = begin(); i != end(); ++i) {
            b->Add(*i);
        }
    }
    else {
        TObject::Browse(b);
    }
}

void ND::TDataVector::ls(Option_t* opt) const {
    ND::TData::ls(opt);
    TROOT::IncreaseDirLevel();
    for (const_iterator v = begin(); v != end(); ++v) {
        (*v)->ls(opt);
    };
    TROOT::DecreaseDirLevel();
}

#ifdef USE_THIS_HERE
ND::TDataVectorIterator::TDataVectorIterator(
    const ND::TDataVector* obj,
    const ND::TDatumVector::iterator current) {
    // Cast to a non-const version since both the const and non-const version
    // of begin are required.
    fObject = const_cast<ND::TDataVector*>(obj);
    fCurrent = current;
    if (fCurrent == fObject->fVector.end()) {
        fCurrent = fObject->fTemporary.begin();
    }
}

bool ND::TDataVectorIterator::operator==(const TDataVectorIterator& rhs) const {
    if (fObject != rhs.fObject) return false;
    if (fCurrent != rhs.fCurrent) return false;
    return true;
}

bool ND::TDataVectorIterator::operator!=(const TDataVectorIterator& rhs) const {
    return !((*this) == rhs);
}

ND::TDataVectorIterator&
ND::TDataVectorIterator::operator++() {
    ++fCurrent;
    if (fCurrent == fObject->fVector.end()) {
        fCurrent = fObject->fTemporary.begin();
    }
    return *this;
}

ND::TDataVectorIterator
ND::TDataVectorIterator::operator++(int) {
    ND::TDataVectorIterator tmp(*this);
    ++(*this);
    return tmp;
}

ND::TDataVectorIterator&
ND::TDataVectorIterator::operator--() {
    if (fCurrent == fObject->fTemporary.begin()) {
        fCurrent = fObject->fVector.end();
    }
    --fCurrent;
    return *this;
}

ND::TDataVectorIterator
ND::TDataVectorIterator::operator--(int) {
    ND::TDataVectorIterator tmp(*this);
    --(*this);
    return tmp;
}

ND::TDataVectorIterator::reference
ND::TDataVectorIterator::operator*() const {
    return *fCurrent;
}

ND::TDataVectorIterator::pointer 
ND::TDataVectorIterator::operator->() const {
    return &(*fCurrent);
}
#endif
