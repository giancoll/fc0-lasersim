
#include "TND280Log.hxx"
#include "TMidasBank.hxx"
#include "TMidasObj.hxx"

ClassImp(ND::TMidasObj);

//_____________________________________________________________________________
    
ND::TMidasObj::TMidasObj(UShort_t rawDataVer,     /* = 0 */
			 UShort_t invalid_code,   /* = 0 */
			 const TMidasBank* parent /* = 0 */)
    :  fParent(parent),
       fRawDataVer(rawDataVer),
       fInvalidCode(invalid_code) {

    // Default constructor

    ND280Trace("TMidasObj: Default ctor at:" << (void*) this);
    this->Connect();
}
//_____________________________________________________________________________
    
ND::TMidasObj::TMidasObj(const ND::TMidasObj& from)
    :  fParent(0),
       fRawDataVer(0),
       fInvalidCode(0) {

    // Copy constructor

    ND280Trace("TMidasObj: Copy ctor at:" << (void*) this);
    this->Clone(from);
}


//_____________________________________________________________________________

ND::TMidasObj::~TMidasObj() {

    this->Disconnect();
    ND280Trace("ND::TMidasObj: dtor at:" << (void*) this);

}

//_____________________________________________________________________________

void ND::TMidasObj::BankDeleted() {

    if ( fParent ) {
	ND280Warn("ND::TMidasObj: Parent object has been deleted which makes this object invalid");
	fParent      = 0;
	fInvalidCode = DeletedParent;
    }
}

//_____________________________________________________________________________

void ND::TMidasObj::Clone(const ND::TMidasObj& from) {

    this->Disconnect();
    this->fParent      = from.fParent;
    this->fRawDataVer  = from.fRawDataVer;
    this->fInvalidCode = from.fInvalidCode;
    this->Connect();

}

//_____________________________________________________________________________

void ND::TMidasObj::Connect() {

    if ( fParent ) {
	if ( fParent->IsNotZombie() ) fParent->ConnectChildObj(this);
	else {
	    ND280Error("TMidasObj: Attempting to connect to a deleted TMidasBank. Please investigate!");
	    fParent      = 0;
	    fInvalidCode = DeletedParent;
	}
    }

}

//_____________________________________________________________________________

void ND::TMidasObj::Disconnect() {

    if ( fParent ) {
        if ( fParent->IsNotZombie() ) fParent->DisconnectChildObj(this);
	else ND280Error("TMidasObj: Attempting to disconnect from a deleted TMidasBank. Please investigate!");
    }
    fParent = 0;
}

//_____________________________________________________________________________

const char* ND::TMidasObj::GetInvalidReason() const {
    if ( this->GetInvalidCode() == NotInvalid )         return "Not invalid"; 
    if ( this->GetInvalidCode() == DeletedParent )      return "Parent TMidasBank deleted"; 
    if ( this->GetInvalidCode() == IteratorOutOfRange ) return "Object produced by out of range iterator"; 
    if ( this->GetInvalidCode() == UnknownInvalid )     return "Object declared invalid but code given was itself invalid"; 
    return "Invalid for some unknown reason (should not happen)"; 
}


//_____________________________________________________________________________

Bool_t ND::TMidasObj::NotValid(const char* calledMethod /*= 0 */) const {

    if ( this->IsValid() ) return false;
    std::string action("access");
    if ( calledMethod ) {
	action = "call method ";
	action += calledMethod;
	action += " of";
    }
    ND280Warn("ND::TMidasObj::Attempting to " << action
	      << " an invalid object (" << this->GetInvalidReason() << ")");
    return true;
}


//_____________________________________________________________________________

void ND::TMidasObj::SetInvalidCode(UShort_t invalid_code) {

    if ( ! this->IsValid() ) {
      ND280Warn("ND::TMidasObj::Attempting to set invalid code to " << invalid_code
		<< " on an already invalid object (" << this->GetInvalidReason() << ")");
      return;
    }
    if (    invalid_code != DeletedParent
	 &&     invalid_code != IteratorOutOfRange)  {
	ND280Warn("ND::TMidasObj::Attempting to set invalid code to " << invalid_code
		  << " which is itself an invalid code" << " must be either "<<DeletedParent<<" or "<<IteratorOutOfRange);
	invalid_code = UnknownInvalid;
    }

    fInvalidCode = invalid_code;
}

