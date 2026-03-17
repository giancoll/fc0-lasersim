// $Id$
//
// Implement the ND::TDataSymLink class.  This is a very simple class so almost
// all of the methods are implemented in line.  However the ClassImp
// macro must be in a separate file and the virtual distructor is also
// implemented here.

#include <TBrowser.h>

#include "TDataSymLink.hxx"

ClassImp(ND::TDataSymLink);

ND::TDataSymLink::~TDataSymLink() {
    // Null the fParent incase there are dangling pointers someplace.  
    fLink = "";
}

// This calls the function for the think that is being linked to.
Bool_t ND::TDataSymLink::IsFolder(void) const {
    Bool_t result =  GetThis()->IsFolder();
    return result;
}

// Called by the browser.  This calls the browser of the class that is being
// linked.  
void ND::TDataSymLink::Browse(TBrowser* b) {
    GetThis()->Browse(b);
}

void ND::TDataSymLink::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "-> " << fLink;
    const ND::TDatum *link = GetThis();
    if (link) {
        std::cout << " <" <<
            link->ClassName() << "(" << link << "):: " 
                  << link->GetFullName()
                  << ">";
    }
    else {
        std::cout << " <does not exist>";
    }
    std::cout << std::endl;
    TROOT::DecreaseDirLevel();
}
