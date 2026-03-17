// $Id$
//
// Implement the ND::TDatum class.  This is a very simple class so almost
// all of the methods are implemented in line.  However the ClassImp
// macro must be in a separate file and the virtual distructor is also
// implemented here.
#include <string.h>

#include <TClass.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TStreamerInfo.h>
#include <TMemberInspector.h>

#include "TDatum.hxx"

ClassImp(ND::TDatum);

ND::TDatum::TDatum() 
    : TNamed("unnamed",TDATUM_TITLE), fParent(NULL) { 
    SetBit(kCanDelete,true);
}

ND::TDatum::TDatum(const char* name, const char* title) 
    : TNamed(name,title), fParent(NULL) { 
    SetBit(kCanDelete,true);
}

ND::TDatum::~TDatum() {
    // Null the fParent incase there are dangling pointers someplace.  
    fParent = NULL;
}

void ND::TDatum::ReassignParentDatum(ND::TDatum *parent) {
    ND::TDatum *old = GetParentDatum();
    
    // Short circuit if the parent is not changed.
    if (old == parent) return;

    if (old) old->RemoveDatum(this);

    fParent = parent;
}

TString ND::TDatum::GetFullName(void) const {
    TString name;
    for (const ND::TDatum *parent = this;
         parent != NULL;
         parent = parent->GetParentDatum()) {
        std::string parentName(parent->GetName());
        if (parentName == "") name.Prepend("unnamed");
        else name.Prepend(parent->GetName());
        name.Prepend("/");
    }
    name.Prepend("/");
    return name;
}

// Find a ND::TDatum object starting from a ND::TDatum using a slash separated
// name.  ND::TDatum objects can't have any down links (remember, the
// ND::TData class is derived so that ND::TDatum objects can contain other
// ND::TDatum objects), so this can only search up.
ND::TDatum* ND::TDatum::RecursiveFind(const char *theName) const {
    if (theName == 0) return NULL;

    // The result of the search.
    ND::TDatum* result = const_cast<ND::TDatum*>(this);

    // Make a local copy of the name so that we can modify it.
    char cname[1024];
    char *name = cname;
    char *slash;
    strncpy(cname,theName,sizeof(cname));
    cname[sizeof(cname)-1] = 0;

    // Check to see if the name is empty.
    if (!(*name)) return result;

    // Make sure that any double slashes "//" occur at the beginning of the
    // name.
    char* subStr = strstr(name+1,"//");
    if (subStr) {
        // There is an internal "//".  Remove it and start over again.
        // Make space in the string.
        int len = strlen(subStr);
        memmove(subStr+1,subStr,len);
        *(subStr+1) = ':';
        result = result->RecursiveFind(name);
        return result;
    }

    // Go to the root of the data, or look for the first one that
    // matchs the slashname. 
    if (*name == '/') {
        ++name;
        if (*name == '/') {
            // The name syntax is //xxxx or //xxxx/yyyy so go to the root of
            // the data tree and then check that the root datum is named xxxx,
            // then do a recursive find from there.  A bare "//" will just go
            // to the root of the tree.
            ++name;
            // Go all the way to the root datum
            result = result->GetRootDatum();
            if (!result) return NULL;
            result = result->GetThis();
            if (!result) return NULL;
            if (!(*name)) return result;
            slash = strchr(name,'/');
            if (slash) {
                *slash = 0;
                if (strcmp(name,result->GetName())) return NULL;
                result = result->RecursiveFind(slash+1);
                if (result) result = result->GetThis();
            }
            else if (strcmp(name,result->GetName())) return NULL;
            return result;
        } 
        else {
            // The name syntax is /xxxx or /xxxx/yyyy so look down the
            // data tree until we find the first occurance of xxxx and
            // then do a recursive find from there.
            slash = strchr(name,'/');
            if (*slash) *slash = 0;
            // Look for the requested datum.
            for (result = const_cast<ND::TDatum*>(this); 
                 result && strcmp(name,result->GetName()) != 0;
                 result = result->GetParentDatum());
            if (result) result = result->GetThis();
            if (!result) return NULL;
            // Now do the rest of the recursion.
            if (slash) result = result->RecursiveFind(slash+1);
            if (result) result = result->GetThis();
            return result;
        }
    }
    else if (*name == '.') {
        ++name;
        if (*name == '.') {
            // The name syntax is ".." so move up one level in the data tree.
            result = result->GetParentDatum();
            if (!result) return NULL;
            result = result->GetThis();
            ++name;
        }
        if (*name == '/') {
            // The name syntax is "./" stay at the current level and look for
            // the rest of the name starting from here.
            ++name;
            result = result->RecursiveFind(name);
            if (result) result = result->GetThis();
            return result;
        }        
        if (*name == 0) return result;
        return NULL;  // Not suppose to happen.
    }
    else if (*name == '~') {
        // The name syntax should be "~/" so move to the root of this tree and
        // look for the rest of the name.
        ++name;
        if (*name != '/') throw EBadName();
        ++name;
        result = result->GetRootDatum();
        if (!result) return NULL;
        result = result->GetThis();
        if (!result) return NULL;
    }

    // Check to see if the name was "//", or "~/".  If it was, just return the
    // result.
    if (!(*name)) return result;

    // Find relative to the current datum.
    slash = strchr(name,'/');
    if (!slash) {
        // The name syntax is xxxx.  No slash found, so just return
        // the datum in this.
        ND::TDatum* datum = static_cast<ND::TDatum*>(result);
        if (!datum) throw EBadConversion();
        result = datum->FindDatum(name);
        if (result) result = result->GetThis();
    }
    else {
        // The name syntax is xxxx/yyyy so zero terminate the name and
        // find it in this.  The result has to be derived from ND::TData
        // since it supports more
        *slash = 0;
        ND::TDatum* datum = static_cast<ND::TDatum*>(result);
        if (!datum) throw EBadConversion();
        result = datum->FindDatum(name);
        if (!result) return NULL;
        result = result->GetThis();
        if (!result) return NULL;
        // Recursively find the rest of the name in the result object.
        result = result->RecursiveFind(slash+1);
        if (result) result = result->GetThis();
    }

    return result;
}

void ND::TDatum::Browse(TBrowser *b) {
    if (b) {
        TClass::AutoBrowse(this,b);
    }
}

void ND::ls_header(const TObject* obj, Option_t* opt) {
    TROOT::IndentLevel();
    std::cout << obj->ClassName() << "(" << obj << "):: ";
    std::cout << obj->GetName();
    if (strstr(opt,"size")) {
        TClass* cls = obj->Class();
        if (!cls) return;
        std::cout << " (" << cls->Size() << " b)";
    }
}

void ND::TDatum::ls(Option_t* opt) const {
    ls_header(this, opt);
    if (strstr(opt,"title")) {
        std::cout << " Title:" << GetTitle();
    }
    std::cout << std::endl;
    if (strstr(opt,"full")) {
        TROOT::IncreaseDirLevel();
        TROOT::IndentLevel();
        std::cout << "Full name: " << GetFullName() << std::endl;
        TROOT::DecreaseDirLevel();
    }
}

// The default FindDatum never finds the object since there is never
// an object to find.
ND::TDatum* ND::TDatum::FindDatum(const char*) {
    return NULL;
}

// Get the datum at the top of the TDatum tree.
ND::TDatum* ND::TDatum::GetRootDatum(void) const {
    const TDatum* root;
    for (root=this;
         root->GetParentDatum() != NULL;
         root = root->GetParentDatum());
    return const_cast<TDatum*>(root);
}

ND::TDatum* ND::TDatum::RemoveDatum(ND::TDatum* element) {
    // Check that the element is actually held by the current object.  If this
    // object is the parent, then reset it's fParent field and return a
    // pointer to the element.  If this object is not the parent, then just
    // return NULL.
    if (element->fParent != this) return NULL;
    element->fParent = NULL;
    return element;
}


ND::TDatumCompareName::TDatumCompareName(const char* name) : fName(name) {
    if (fName == ":") fName = "unnamed";
}

ND::TDatumCompareName::~TDatumCompareName() {}

bool ND::TDatumCompareName::operator () (const TDatum* elem) {
    if (fName == "unnamed" && TString(elem->GetName()) == "") return true;
    return (fName == elem->GetName());
}
