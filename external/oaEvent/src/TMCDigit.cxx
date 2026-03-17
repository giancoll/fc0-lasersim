#include "TMCDigit.hxx"

ClassImp(ND::TMCDigit);

ND::TMCDigit::TMCDigit() {}

ND::TMCDigit::TMCDigit(const ND::TMCDigit::ContributorContainer& contrib) 
    : fContributors(contrib) {}

ND::TMCDigit::~TMCDigit() {}
