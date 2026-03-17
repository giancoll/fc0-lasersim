#include "TAlignmentId.hxx"

ClassImp(ND::TAlignmentId);

ND::TAlignmentId::~TAlignmentId() {}

ND::TAlignmentId::TAlignmentId() {}

ND::TAlignmentId::TAlignmentId(const TAlignmentId& hc) 
    : TSHAHashValue(hc), fDocString(hc.fDocString) {}

ND::TAlignmentId::TAlignmentId(const TSHAHashValue& hc) 
    : TSHAHashValue(hc), fDocString("") {}

ND::TAlignmentId::TAlignmentId(unsigned int hc[5]) 
    : TSHAHashValue(hc) {}

ND::TAlignmentId::TAlignmentId(unsigned int hc0, 
                               unsigned int hc1, 
                               unsigned int hc2, 
                               unsigned int hc3, 
                               unsigned int hc4) 
    : TSHAHashValue(hc0, hc1, hc2, hc3, hc4) {}
