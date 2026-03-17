#include <RVersion.h>

#ifdef __CINT__

#pragma link C++ class ND::TReconState+;
#pragma link C++ class ND::THandle<ND::TReconState>+;

#if ROOT_VERSION_CODE < ROOT_VERSION(5,34,0)
#pragma link C++ class ND::TMReconState-;
#pragma link C++ class ND::TMEDepositState-;
#pragma link C++ class ND::TMPositionState-;
#pragma link C++ class ND::TMDirectionState-;
#pragma link C++ class ND::TMPositionDirectionState-;
#pragma link C++ class ND::TMConeState-;
#pragma link C++ class ND::TMCurvatureState-;
#pragma link C++ class ND::TMPosDirCurvState-;
#pragma link C++ class ND::TMWidthState-;
#pragma link C++ class ND::TMMomentumState-;
#pragma link C++ class ND::TMChargeState-;
#endif

#endif
