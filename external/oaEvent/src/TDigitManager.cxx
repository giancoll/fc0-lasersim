#include <algorithm>

#include "TND280Event.hxx"
#include "TEventFolder.hxx"
#include "TOADatabase.hxx"
#include "TDigitManager.hxx"
#include "TND280Log.hxx"

ND::TDigitManager::TDigitManager() 
    : fPersistentDigits(false) {}

ND::TDigitManager::~TDigitManager() {}

void ND::TDigitManager::RegisterFactory(ND::TDigitFactory* factory) {
    std::string name = factory->GetName();
    if (fFactories.find(name) != fFactories.end()) {
        throw ND::EMultipleDigitFact();
    }
    fFactories[name] = factory;
}

bool ND::TDigitManager::FactoryAvailable(std::string type) const {
    return (fFactories.find(type) != fFactories.end());
}

ND::TDigitFactory& ND::TDigitManager::GetFactory(std::string type) {
    if (!FactoryAvailable(type)) {
	throw ND::EDigitFactNotFound();
    }

    return *fFactories[type];
}

ND::THandle<ND::TDigitContainer> 
ND::TDigitManager::CacheDigits(std::string type) {
    // Check to see if the digits already exist in the current event.
    ND::TND280Event* event = TEventFolder::GetCurrentEvent();
    if (!event) {
        throw EDigitEventMissing();
    }
    return CacheDigits(*event,type);
}

ND::THandle<ND::TDigitContainer> 
ND::TDigitManager::CacheDigits(ND::TND280Event& event,
                               std::string type) {

    ND::THandle<ND::TDigitContainer> digits = 
        event.Get<ND::TDigitContainer>("~/digits/"+type);
    if (digits) return digits;

    // The digits don't exist in the event, so try to generate them using the
    // TDigitFactory.
    ND::TDigitFactory* factory = fFactories[type];
    if (!factory) return ND::THandle<ND::TDigitContainer>();
    digits = ND::THandle<ND::TDigitContainer>(factory->MakeDigits());
    if (!digits) {
        ND280Warn("The " << factory->GetName() << " digit factory failed");
        return ND::THandle<ND::TDigitContainer>();
    }
    digits->SetName(type.c_str());

    // Save the digits in the current event.
    ND::THandle<ND::TDataVector> d = event.Get<ND::TDataVector>("~/digits");
    if (!fPersistentDigits) d->AddTemporary(digits);
    else d->AddDatum(digits);

    return digits;
}

ND::THandle<ND::TDigitContainer> 
ND::TDigitManager::FindDigits(const TDigitProxy& proxy) {
    std::string name = TDigitProxy::ConvertType(proxy.GetProxyType());

    // Check to see if the TDigitContainer is already part of the event.
    ND::THandle<ND::TDigitContainer> digits = CacheDigits(name);
    return digits;
}

ND::TDigit* ND::TDigitManager::GetDigit(const TDigitProxy& proxy) {
    std::string name;

    // Check to see if the proxy has already cached the pointer.
    TDigit* pointer = proxy.GetProxyCache();
    if (pointer) return pointer;

    // Find the digits in the event.  This will regenerate the TDigitContainer
    // if necessary (and possible).
    ND::THandle<ND::TDigitContainer> digits = FindDigits(proxy);
    if (!digits) throw EDigitNotAvailable();

    // Get the offset of the digit inside of the container and throw an
    // exception if the offset is invalid.
    unsigned int offset = proxy.GetProxyOffset();
    if (!(offset<digits->size())) throw EDigitNotFound();

    pointer = digits->at(offset);

    // Check that the proxy salt matches the container and digit.  Throw an
    // exception if the salt doesn't match.
    if (!proxy.CheckSalt(digits->GetSignature(),pointer))
        throw EDigitMismatch();

    /// Cache the container and digit for future lookup.
    proxy.SetProxyCache(pointer,ND::GetPointer(digits));

    return pointer;
}

ND::TDigitFactory::TDigitFactory(std::string name) 
    : fName(name) {}
    
ND::TDigitFactory::~TDigitFactory() {}

std::string ND::TDigitFactory::GetName() {
    return fName;
}
