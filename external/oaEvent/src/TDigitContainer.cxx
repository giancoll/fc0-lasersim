#include "TDigitContainer.hxx"
#include "TDigitHeader.hxx"

ClassImp(ND::TDigitContainer);

ND::TDigitContainer::TDigitContainer(const char* name, const char* title)
    : TDatum(name,title), fSignature(0) {}

ND::TDigitContainer::~TDigitContainer() {
    for (ND::TDigitContainer::iterator d = begin();
         d != end();
         ++d) {
        delete (*d);
    }
    for (std::vector<ND::TDigitHeader*>::iterator h = fHeaders.begin();
         h != fHeaders.end();
         ++h) {
        delete (*h);
    }
    fHeaders.clear();
    clear();
}

ND::TDigitHeader * ND::TDigitContainer::GetHeader(int i) const {
    return fHeaders.at(i);
}

void ND::TDigitContainer::AddHeader(ND::TDigitHeader* hdr) {
    fHeaders.push_back(hdr);
}

unsigned int ND::TDigitContainer::GetSignature() const {
    if (fSignature == 0) {
        // Calculate the signature using a modified Fowler, Noll, and Vo hash
        // type 1a (FNV-1a).  The signature is calculated so that it will be
        // equal if two TDigitContainers that have the same name, title and
        // data.  This gives the oaEvent code freedom to delete
        // TDigitContainers which can be regenerated at a later point, while
        // making sure that the regenerated TDigitContainers have the same
        // data.
        const int prime = 0x01000193;  // The FNV-1 value;
        const int offset = 0x811c9dc5; // The FNV-1 value;
        fSignature = offset;

        // Add the name of the TDigitContainer
        for (const char* c = GetName(); *c != 0; ++c) {
            fSignature *= prime;
            fSignature ^= (unsigned int) *c;
        }

        // Add the title of the TDigitContainer
        for (const char* c = GetTitle(); *c != 0; ++c) {
            fSignature *= prime;
            fSignature ^= (unsigned int) *c;
        }

        // Add the channel id for each digit.
        for (TDigitContainer::const_iterator d = begin();
             d != end();
             ++d) {
            unsigned int v = (*d)->GetChannelId().AsUInt();
            fSignature *= prime;
            fSignature ^= (unsigned int) (v & 0x000000FF);
            fSignature *= prime;
            fSignature ^= (unsigned int) ((v & 0x0000FF00)>> 8);
            fSignature *= prime;
            fSignature ^= (unsigned int) ((v & 0x00FF0000)>> 16);
            fSignature *= prime;
            fSignature ^= (unsigned int) ((v & 0xFF000000)>> 24);
        }
    }

    return fSignature;
}

void ND::TDigitContainer::ls(Option_t* opt) const {
    ND::TDatum::ls(opt);
    std::string option(opt);
    TROOT::IncreaseDirLevel();
    TROOT::IndentLevel();
    std::cout << "Signature: " << GetSignature() << std::endl;
    for (unsigned int i = 0; i< GetHeaderCount(); ++i) {
        ND::TDigitHeader *const hdr = GetHeader(i);
        hdr->ls(opt);
    }
    TROOT::IndentLevel();
    std::cout << "Digits: " << size() << std::endl;
    if (option.find("dump") != std::string::npos
        || option.find("digits") != std::string::npos) {
        for (TDigitContainer::const_iterator v = begin();
             v != end();
             ++v) {
            (*v)->ls(opt);
        }
    }
    TROOT::DecreaseDirLevel();
}
