#include <sstream>
#include <string>

#include "TDigitProxy.hxx"
#include "TDigitManager.hxx"
#include "TOADatabase.hxx"

ClassImp(ND::TDigitProxy);

ND::TDigitProxy::~TDigitProxy() {}

ND::TDigitProxy::TDigitProxy() 
  : fDigitSignature(0), fDigit(NULL), fContainer(NULL) {}

ND::TDigitProxy::TDigitProxy(const ND::TDigitContainer& container,
                             unsigned int offset) 
    : fDigitSignature(0), fDigit(NULL), fContainer(NULL) {

    int type = TDigitProxy::ConvertName(container.GetName());
    SetProxyType(type);
    if (container.size() <= offset) throw EDigitNotFound();
    ND::TDigit* digit = container[offset];
    unsigned int cid = digit->GetChannelId().AsUInt();
    unsigned int salt = container.GetSignature()+cid;
    SetProxySalt(salt);
    SetProxyOffset(offset);
    SetProxyCache(digit,NULL);
}

ND::TDigitProxy::TDigitProxy(const ND::TDigitContainer& container,
                             const ND::TDigitContainer::iterator index) 
    : fDigitSignature(0), fDigit(NULL), fContainer(NULL) {

    int type = TDigitProxy::ConvertName(container.GetName());
    SetProxyType(type);
    int offset = index-container.begin();
    if (offset<0 || container.size()<=(unsigned)offset) throw EDigitNotFound();
    ND::TDigit* digit = container[offset];
    unsigned int cid = digit->GetChannelId().AsUInt();
    unsigned int salt = container.GetSignature()+cid;
    SetProxySalt(salt);
    SetProxyOffset(offset);
    SetProxyCache(digit,NULL);
}

ND::TDigitProxy::TDigitProxy(int proxy) 
    : fDigitSignature(proxy), fDigit(NULL), fContainer(NULL) {}

ND::TDigit* ND::TDigitProxy::operator*() const {
    return ND::TOADatabase::Get().Digits().GetDigit(*this);
}

ND::TDigitContainer& ND::TDigitProxy::GetContainer() const {
    ND::TOADatabase::Get().Digits().GetDigit(*this);
    return *fContainer;
}

enum ND::TDigitProxy::ProxyType ND::TDigitProxy::ConvertName(std::string name) {
    if (name == "p0d") return kP0D;
    if (name == "tpc") return kTPC;
    if (name == "fgd") return kFGD;
    if (name == "ecal") return kECal;
    if (name == "smrd") return kSMRD;
    if (name == "ingrid") return kINGRID;
    if (name == "sfg") return kSFG;
    if (name == "hat") return kHAT;
    if (name == "tof") return kTOF;
    if (name == "test") return kTest;
    return kInvalid;
}

std::string ND::TDigitProxy::ConvertType(int type) {
    std::string name;

    // Find the name of the TDigitContainer based on the proxy type.
    switch (type) {
    case kTest: name = "test"; break;
    case kP0D: name = "p0d"; break;
    case kTPC: name = "tpc"; break;
    case kFGD: name = "fgd"; break;
    case kECal: name = "ecal"; break;
    case kSMRD: name = "smrd"; break;
    case kINGRID: name = "ingrid"; break;
    case kSFG: name = "sfg"; break;
    case kHAT: name = "hat"; break;
    case kTOF: name = "tof"; break;
    case kInvalid: name = "invalid"; break;
    default:
        throw EDigitTypeInvalid();
    }

    return name;
}

bool ND::TDigitProxy::CheckSalt(unsigned int signature,
                                const ND::TDigit* digit) const {
    unsigned int trial = (signature + digit->GetChannelId().AsUInt())%1024;
    return trial == GetProxySalt();
}

enum ND::TDigitProxy::ProxyType ND::TDigitProxy::GetProxyType() const {
    unsigned sig = (fDigitSignature & 0xF8000000u) >> 27;
    return (ND::TDigitProxy::ProxyType) sig;
}

unsigned int ND::TDigitProxy::GetProxySalt() const {
    unsigned int salt = (fDigitSignature & 0x07FE0000u) >> 17;
    return salt;
}

unsigned int ND::TDigitProxy::GetProxyOffset() const {
    unsigned int offset = (fDigitSignature & 0x0001FFFF);
    return offset;
}

ND::TDigit* ND::TDigitProxy::GetProxyCache() const {
    return fDigit;
}

void ND::TDigitProxy::SetProxyCache(TDigit* digit,
                                    TDigitContainer* container) const {
    fDigit = digit;
    fContainer = container;
}

void ND::TDigitProxy::SetProxyType(int type) {
    fDigitSignature = (fDigitSignature & 0x07FFFFFFu)|((type & 0x001Fu) << 27);
}

void ND::TDigitProxy::SetProxySalt(int salt) {
    fDigitSignature = (fDigitSignature & 0xF801FFFFu)|((salt & 0x03FFu) << 17);
}

void ND::TDigitProxy::SetProxyOffset(int off) {
    if (off & 0xFFFE0000u) off = 0x1FFFFu;
    if (off == 0x1FFFFu) ND280Severe("Invalid proxy has been created.");
    fDigitSignature = (fDigitSignature & 0xFFFE0000u) | (off & 0x1FFFFu);
}

bool ND::TDigitProxy::IsValid() const {
    if (GetProxyOffset() == 0x1FFFFu) return false;
    if (GetProxyType() == kTest) return true;
    if (GetProxyType() == kP0D) return true;
    if (GetProxyType() == kTPC) return true;
    if (GetProxyType() == kFGD) return true;
    if (GetProxyType() == kECal) return true;
    if (GetProxyType() == kSMRD) return true;
    if (GetProxyType() == kINGRID) return true;
    if (GetProxyType() == kSFG) return true;
    if (GetProxyType() == kHAT) return true;
    if (GetProxyType() == kTOF) return true;
    return false;
}

std::string ND::TDigitProxy::AsString() const {
    std::ostringstream out;
    try {
        out << ConvertType(GetProxyType()) << ":";
    }
    catch (...) {
        out << "invalid:";
    }
    unsigned int offset = GetProxyOffset();
    if (offset == 0x1FFFFu) out << " invalid offset";
    else out << " " << offset;
    return out.str();
}

bool ND::TDigitProxy::operator == (const ND::TDigitProxy& rhs) const {
    return fDigitSignature == rhs.fDigitSignature;
}
