#ifdef __CINT__
#pragma link C++ class ND::TCorrValues+;
#pragma read sourceClass="ND::TCorrValues" version="[1]"              \
     source="TVectorT<double> fVector"                                  \
     targetClass="ND::TCorrValues" target="fVector"                     \
     code="{fVector=onfile.fVector;}"
#pragma read sourceClass="ND::TCorrValues" version="[1]"                \
     source="TMatrixTSym<double> fMatrix"                               \
     targetClass="ND::TCorrValues" target="fMatrix"                     \
     code="{fMatrix=onfile.fMatrix;}"
#endif

