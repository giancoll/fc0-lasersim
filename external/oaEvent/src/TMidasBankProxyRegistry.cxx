#include "TND280Log.hxx"
#include "TMidasBankProxy.hxx"
#include "TMidasBankProxyRegistry.hxx"

ClassImp(ND::TMidasBankProxyRegistry);

ND::TMidasBankProxyRegistry* ND::TMidasBankProxyRegistry::fInstance = 0;

//_____________________________________________________________________________
    
ND::TMidasBankProxyRegistry::TMidasBankProxyRegistry() {

   // Default constructor

    ND280Trace("TMidasBankProxyRegistry: Default ctor at:" << (void*) this);
    
}


//_____________________________________________________________________________

ND::TMidasBankProxyRegistry::~TMidasBankProxyRegistry() {

    ND280Trace("ND::TMidasBankProxyRegistry: dtor at:" << (void*) this);

   // dtor is NOT responsible for destroying elements of the list;
   // they are created statically and are thus automagically
   // destroyed when the process ends

}

//_____________________________________________________________________________

void ND::TMidasBankProxyRegistry::Print() {

    ND280Log("Contents of Singleton TMidasBankProxyRegistry:-");
    std::list<TMidasBankProxy*>::iterator itr(fMidasBankProxyTable.begin()),
	                                     itrEnd(fMidasBankProxyTable.end());
    while ( itr != itrEnd ) {
	TMidasBankProxy& proxy = **itr;
	ND280Log("  " << proxy.GetMidasBankMasks() << "  ->  " << proxy.GetName());
	++itr;
    }
}

//_____________________________________________________________________________

void ND::TMidasBankProxyRegistry::Register(TMidasBankProxy *proxy)
{
    const std::string mask = proxy->GetMidasBankMasks(0);
    // Check that the proxy has not already been registered
    if (this->LookUp(mask)) {
        ND280Warn("TMidasBankProxyRegistry:The bank mask '" << mask << "' is already registered!\n"
    	      << "Duplicate masks are not allowed!\n");
        return;
    }
    else {
      // Do insertions in alphabetical order
      std::list<TMidasBankProxy*>::iterator itr(fMidasBankProxyTable.begin()),
	                                       itrEnd(fMidasBankProxyTable.end());
      for (; itr != itrEnd; ++itr) if ( mask < (*itr)->GetMidasBankMasks() ) break;
      fMidasBankProxyTable.insert(itr,proxy);
  
      ND280Trace("TMidasBankProxyRegistry: Registered mask " << mask 
		 << " proxy @ " << proxy);
    }
  }

//_____________________________________________________________________________

ND::TMidasBankProxy* ND::TMidasBankProxyRegistry::LookUp(const std::string midasBankName) const
{
    // Look up matching proxy in the table.
    std::list<TMidasBankProxy*>::const_iterator itr(fMidasBankProxyTable.begin()),
                                                itrEnd(fMidasBankProxyTable.end());
    for (; itr != itrEnd; ++itr) if ((*itr)->MatchesMidasBankName(midasBankName)) return *itr;
    return 0;
}


//_____________________________________________________________________________

ND::TMidasBankProxyRegistry& ND::TMidasBankProxyRegistry::Instance() 
{
  if (fInstance==0) {
    fInstance = new TMidasBankProxyRegistry();
  }
  return *fInstance;
}
