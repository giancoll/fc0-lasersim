#include "TND280Context.hxx"

#include <ctime>
#include <iostream>

const int ND::TND280Context::Invalid = 0xDEADBEEF;

ClassImp(ND::TND280Context);

ND::TND280Context::~TND280Context() {}

ND::TND280Context::TND280Context()
    : fPartition(Invalid),
      fRun(Invalid), fSubRun(Invalid), fEvent(Invalid),
      fSpill(Invalid), fTimeStamp(Invalid) {}

ND::TND280Context::TND280Context(int partition,
                                 int run, int subRun, int event,
                                 int spill, int stamp)
    : fPartition(partition),
      fRun(run), fSubRun(subRun), fEvent(event),
      fSpill(spill), fTimeStamp(stamp) {}

int ND::TND280Context::GetPartition() const {return fPartition;}
int ND::TND280Context::GetRun() const {return fRun;}
int ND::TND280Context::GetSubRun() const {return fSubRun;}
int ND::TND280Context::GetEvent() const {return fEvent;}
int ND::TND280Context::GetSpill() const {return fSpill;}
int ND::TND280Context::GetTimeStamp() const {return fTimeStamp;}
std::string ND::TND280Context::GetTimeStampString() const {
    std::time_t t = GetTimeStamp();
    char buf[256];
    std::strftime(buf,sizeof(buf),"'%Y-%m-%d %H:%M:%S'",std::gmtime(&t));
    return std::string(buf);
}

void ND::TND280Context::SetPartition(int v) {fPartition = v;}
void ND::TND280Context::SetRun(int v) {fRun = v;}
void ND::TND280Context::SetSubRun(int v) {fSubRun = v;}
void ND::TND280Context::SetEvent(int v) {fEvent = v;}
void ND::TND280Context::SetSpill(int v) {fSpill = v;}
void ND::TND280Context::SetTimeStamp(int v) {fTimeStamp = v;}

bool ND::TND280Context::IsMC() const {
    if (fPartition == Invalid) return false;
    if (fPartition & kMCData) return true;
    return false;
}

bool ND::TND280Context::IsDetector() const {
    if (fPartition == Invalid) return false;
    if (fPartition & kMCData) return false;
    return true;
}

bool ND::TND280Context::Valid() const {
    if (GetPartition()!=ND::TND280Context::Invalid) return true;
    if (GetRun()!=ND::TND280Context::Invalid)  return true;
    if (GetSubRun()!=ND::TND280Context::Invalid) return true;
    if (GetEvent()!=ND::TND280Context::Invalid) return true;
    if (GetSpill()!=ND::TND280Context::Invalid) return true;
    if (GetTimeStamp()!=ND::TND280Context::Invalid) return true;
    return false;
}

std::ostream& operator<<(std::ostream& s, const ND::TND280Context& c) {
    s << "<";
    bool valid = false;
    if (c.GetPartition()!=ND::TND280Context::Invalid) {
        s << "P: " << c.GetPartition();
        valid = true;
    }
    if (c.GetRun()!=ND::TND280Context::Invalid) {
        if (valid) s << " ";
        s << "R: " << c.GetRun();
        valid = true;
    }
    if (c.GetSubRun()!=ND::TND280Context::Invalid) {
        if (valid) s << " ";
        s << "SR: " << c.GetSubRun();
        valid = true;
    }
    if (c.GetEvent()!=ND::TND280Context::Invalid) {
        if (valid) s << " ";
        s << "E: " << c.GetEvent();
        valid = true;
    }
    if (c.GetSpill()!=ND::TND280Context::Invalid) {
        if (valid) s << " ";
        s << "S: " << c.GetSpill();
        valid = true;
    }
    if (c.GetTimeStamp()!=ND::TND280Context::Invalid) {
        if (valid) s << " ";
        std::time_t t = c.GetTimeStamp();
        char buf[64];
        std::strftime(buf,sizeof(buf),"%y/%m/%d %X %Z",std::gmtime(&t));
        s << "T: " << buf;
        valid = true;
    }
    if (valid) {
        if (c.IsMC()) s << " MC";
        else if (c.IsDetector()) s << " DET";
    }
    else {
        s << "Invalid Context";
    }
    s << ">";
    return s;
}
