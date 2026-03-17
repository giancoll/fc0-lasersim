#include <iostream>
#include <map>
#include <set>

#include <TROOT.h>
#include <TClass.h>

#include "THandle.hxx"
#include "THandleHack.hxx"
#include "TND280Log.hxx"

namespace {
    int gHandleBaseCount = 0;
    int gLastHandleCount = 0;
    std::set<ND::THandleBase*> *gHandleSet = NULL;
}

ClassImp(ND::THandleBase);
ND::THandleBase::THandleBase() : fCount(0), fHandleCount(0) {
    ++gHandleBaseCount;
    if (gHandleSet) gHandleSet->insert(this);
}
ND::THandleBase::~THandleBase() {
    --gHandleBaseCount;
    if (gHandleSet) gHandleSet->erase(this);
}

ClassImp(ND::THandleBaseDeletable);
ND::THandleBaseDeletable::THandleBaseDeletable() 
    : fObject(NULL) { }
ND::THandleBaseDeletable::THandleBaseDeletable(TObject* pointee)
    : fObject(pointee) { }
ND::THandleBaseDeletable::~THandleBaseDeletable() {
    DeleteObject();
}
void ND::THandleBaseDeletable::DeleteObject() {
    if (!fObject) return;
    // Actually delete the object.
    if (IsOwner()) delete fObject;
    fObject = NULL;
}

ClassImp(ND::THandleBaseUndeletable);
ND::THandleBaseUndeletable::THandleBaseUndeletable() : fObject(NULL) { }
ND::THandleBaseUndeletable::THandleBaseUndeletable(TObject* pointee)
    : fObject(pointee) { }
ND::THandleBaseUndeletable::~THandleBaseUndeletable() {
    DeleteObject();
}
void ND::THandleBaseUndeletable::DeleteObject() {
    fObject = NULL;  // Just set the object pointer to NULL;
}

bool ND::CleanHandleRegistry(bool) {
    bool result = (gHandleBaseCount==gLastHandleCount);
    if (!result) {
        ND280Log("CleanHandleRegistry::"
                 << " Handle Count: " << gHandleBaseCount 
                 << " Change: " << gHandleBaseCount - gLastHandleCount);
        gLastHandleCount = gHandleBaseCount;
    }
    return result;
}

void ND::DumpHandleRegistry() {
    if (!gHandleSet) return;
    if (gHandleSet->empty()) return;
    ND280Log("Existing handles: " << gHandleSet->size());
    ND::TND280Log::IncreaseIndentation();
    for (std::set<ND::THandleBase*>::iterator h = gHandleSet->begin();
         h != gHandleSet->end();
         ++h) {
        ND::THandleBase* handleBase = *h;
        ND280Log(std::hex << "(0x" << handleBase << ")");
        if (handleBase) {
            ND::TND280Log::IncreaseIndentation();
            TObject *object = handleBase->GetObject();
            ND280Log(std::hex << "-> (0x" << object << ")");
            if (object) {
                ND::TND280Log::IncreaseIndentation();
                ND280Log("Class: " << object->ClassName());
                ND280Log("Name: " << object->GetName());
                ND::TND280Log::DecreaseIndentation();
                if (ND::TND280Log::GetDebugLevel()>ND::TND280Log::ErrorLevel) {
                    object->ls();
                }
            }
            ND::TND280Log::DecreaseIndentation();
        }
    }
    ND::TND280Log::DecreaseIndentation();
} 

void ND::EnableHandleRegistry(bool enable) {
    if (enable && !gHandleSet) {
        ND280Log("Enable the handle registry");
        gHandleSet = new std::set<ND::THandleBase*>;
    }
    else { 
        ND280Log("Disable the handle registry");
        delete gHandleSet;
        gHandleSet = NULL;
    }
}

ClassImp(ND::TVHandle);
ND::TVHandle::TVHandle() {Default(NULL);}
ND::TVHandle::~TVHandle() {}

void ND::TVHandle::Default(ND::THandleBase* handle) {
    fHandle = handle;
    SetBit(kWeakHandle,false);
    if (fHandle) {
        fHandle->CheckHandle();
        fHandle->IncrementReferenceCount();
        fHandle->IncrementHandleCount();
    }
}

void ND::TVHandle::Link(const ND::TVHandle& rhs) {
    // Copy the handle.
    fHandle = rhs.fHandle;
    if (!fHandle) return;
    fHandle->CheckHandle();
    fHandle->IncrementHandleCount();
    if (IsWeak()) return;
    fHandle->IncrementReferenceCount();
}

void ND::TVHandle::Unlink() {
    if (!fHandle) return;
    fHandle->CheckHandle();
    if (!IsWeak()) fHandle->DecrementReferenceCount();
    fHandle->DecrementHandleCount();
    CheckSurvival();
    fHandle = NULL;
}

void ND::TVHandle::MakeWeak() {
    if (IsWeak()) return;
    SetBit(kWeakHandle,true);
    // Decrement the reference count to the object, but leave the handle count
    // unchanged.
    if (!fHandle) return;
    fHandle->CheckHandle();
    fHandle->DecrementReferenceCount();
    CheckSurvival();
}

void ND::TVHandle::MakeLock() {
    if (!IsWeak()) return;
    SetBit(kWeakHandle,false);
    // Increment the reference count to the object, but leave the handle count
    // unchanged, but only if there is a valid handle, and a valid object.
    if (!fHandle) return;
    if (!fHandle->GetObject()) return;
    fHandle->CheckHandle();
    fHandle->IncrementReferenceCount();
}

void ND::TVHandle::CheckSurvival() {
    // The handle doesn't exist, so just return.
    if (!fHandle) return;
    // Check for old handles.
    fHandle->CheckHandle();
    // The handle counter is zero so nothing (no strong, or weak handles) is
    // using this THandleBase and it should be deleted.  This also deletes the
    // object.
    if (fHandle->GetHandleCount() < 1) {
        fHandle->DeleteObject();
        delete fHandle;
        fHandle = NULL;
        return;
    }
    // The reference counter is zero, so no strong handles are referencing the
    // object.  Delete the object, but leave the THandleBase.
    if (fHandle->GetReferenceCount() < 1) {
        fHandle->DeleteObject();
    }
}

TObject* ND::TVHandle::GetPointerValue() const {
    if (!fHandle) return NULL;
    return fHandle->GetObject();
}

void ND::TVHandle::Release(void) {
    if (!fHandle) return;
    fHandle->Release();
}

bool ND::TVHandle::operator == (const ND::TVHandle& rhs) const {
    if (fHandle == rhs.fHandle) return true;
    if (!fHandle) return false;
    if (!rhs.fHandle) return false;
    return (fHandle->GetObject() == rhs.fHandle->GetObject());
}

void ND::TVHandle::ls(Option_t *opt) const {
    TROOT::IndentLevel();
    std::cout << ClassName() << "(" << this << "):: ";
    if (strstr(opt,"size")) {
        TClass* cls = Class();
        if (!cls) return;
        std::cout << " (" << cls->Size() << " b)";
    }
    if (fHandle) {
        std::cout << " Refs: " << fHandle->GetReferenceCount();
        std::cout << " (" << fHandle->GetHandleCount() << ")";
        if (IsWeak()) std::cout << " (weak)";
        else if (fHandle->IsOwner()) std::cout << " (owner)";
        else std::cout << " (released)";
    }
    std::cout << std::endl;
    TROOT::IncreaseDirLevel();
    const TObject* ptr = GetPointerValue();
    if (ptr) ptr->ls(opt);
    TROOT::DecreaseDirLevel();
}
