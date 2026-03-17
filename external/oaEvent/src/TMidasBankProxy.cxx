#include "TND280Log.hxx"
#include "TMidasBank.hxx"
#include "TMidasBankProxy.hxx"

#include <TCollection.h>
#include <TObject.h>
#include <TObjArray.h>
#include <TString.h>

ClassImp(ND::TMidasBankProxy);

//_____________________________________________________________________________
    
ND::TMidasBankProxy::TMidasBankProxy(const std::string name   /*  = "TMidasBankProxy"  */,
				     const std::string masks   /*  = "none"             */) :
    fName(name) {

   // Default constructor

    // Split semi-colon separated list of names ito individual strings.
    TString str(masks);
    TObjArray* arr(str.Tokenize(";"));
    TIter itr(arr);
    while ( TObject* obj_str = itr() ) { fMidasBankMasks.push_back(obj_str->GetName()); }
    delete arr;
    arr = 0;

    ND280Trace("TMidasBankProxy: Default ctor for masks '" << masks << "' at:" << (void*) this);
    
}


//_____________________________________________________________________________

ND::TMidasBankProxy::~TMidasBankProxy() {

    ND280Trace("ND::TMidasBankProxy: dtor at:" << (void*) this);

}


//_____________________________________________________________________________

ND::TMidasBank* ND::TMidasBankProxy::CreateMidasBank(
			 const ULong64_t *bank  /*  = NULL                */,
			 const char* title      /*  = TMIDAS_BANK_TITLE   */) const {
    
    ND::TMidasBank* rdb = new ND::TMidasBank(bank,title);
    ND280Trace("ND::TMidasBankProxy: Creating a raw TMidasBank named " << rdb->GetName() 
	      << " at:" << (void*) rdb);
    return rdb;

}

//_____________________________________________________________________________

std::string ND::TMidasBankProxy::GetMidasBankMasks(Int_t index /*= -1 */) const {

    std::string masks;
    std::list<std::string>::const_iterator itr(fMidasBankMasks.begin()),
	                                   itrEnd(fMidasBankMasks.end());
    Int_t entry = 0;
    while ( itr != itrEnd ) {
	if ( index < 0 || index == entry ) {
	    if ( masks.size() ) masks += ";";
	    masks += *itr;
	}
	++entry;
	++itr;
    }
    return masks;

}



//_____________________________________________________________________________

Bool_t ND::TMidasBankProxy::MatchesMidasBankName(const std::string name) const {


    std::list<std::string>::const_iterator itr(fMidasBankMasks.begin()),
	                                   itrEnd(fMidasBankMasks.end());
    while ( itr != itrEnd ) {
	std::string mask = *(itr++);
	if ( mask.size() != name.size() ) continue;
	Bool_t matches = true;
        for (unsigned i_chr = 0; i_chr < mask.size(); ++i_chr) {
	    if ( mask[i_chr] != '*' and mask[i_chr] != name[i_chr] ) matches = false;
	}
	if ( matches ) return true;
    }
    return false;
    

}

