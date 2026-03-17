#ifndef TFGDDigitHeader_hxx_seen
#define TFGDDigitHeader_hxx_seen

#include <vector>
#include <TDigitHeader.hxx>

namespace ND {
        class TCMBDigitHeader;
    class TFGDDigitHeader;
};

/// Header information for a single CMB (Crate Master Board).
class ND::TCMBDigitHeader{

public:

    TCMBDigitHeader();

    TCMBDigitHeader(int Minicrate, unsigned int TriggerCounter, unsigned int Timestamp,
                    unsigned int FecError, unsigned int LosFlag, unsigned int SynFail);

    virtual ~TCMBDigitHeader(){};

    int GetMinicrate() const {return fMinicrate;};

    unsigned int GetCMBTriggerCounter() const {return fTriggerCounter;};
    unsigned int GetCMBTimestamp() const {return fTimestamp;};
    
    unsigned int GetCMBFecError() const {return fFecError;}
    unsigned int GetCMBLosFlag() const {return fLosFlag;}
    unsigned int GetCMBSynFail() const {return fSynFail;}

private:

    int fMinicrate;

    unsigned int fTriggerCounter;
    unsigned int fTimestamp;

    unsigned int fFecError;
    unsigned int fLosFlag;
    unsigned int fSynFail;
    
    ClassDef(TCMBDigitHeader, 1);
};

/// Header information for FGD:
/// essentially just a vector of CMB header information.
class ND::TFGDDigitHeader : public ND::TDigitHeader{

public:

    TFGDDigitHeader();

    virtual ~TFGDDigitHeader();

    std::vector<ND::TCMBDigitHeader> const GetCMBHeaders() const {return fCMBHeaders;}

    ND::TCMBDigitHeader const  GetCMBHeader(int cmb) const {

        // Find the right header, else return blank.
        for(unsigned int i = 0 ; i < fCMBHeaders.size(); i++){
            if(fCMBHeaders[i].GetMinicrate() == cmb)
                return fCMBHeaders[i];
        }
        return ND::TCMBDigitHeader();
    }

    void AddCMBHeader(ND::TCMBDigitHeader header){
        fCMBHeaders.push_back(header);        
    }

private:

    std::vector<ND::TCMBDigitHeader> fCMBHeaders;

    ClassDef(TFGDDigitHeader, 1);

};
#endif
