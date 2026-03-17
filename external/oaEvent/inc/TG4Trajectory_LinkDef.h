#ifdef __CINT__
#pragma link C++ class ND::TG4Trajectory+;
#pragma link C++ class ND::TG4TrajectoryContainer+;
#pragma link C++ class ND::THandle<ND::TG4Trajectory>+;
#pragma link C++ class ND::THandle<ND::TG4TrajectoryContainer>+;
#pragma read sourceClass="ND::TG4Trajectory" version="[-3]"        \
     source="TLorentzVector fInitialMomentum"                                 \
     targetClass="ND::TG4Trajectory"                               \
     target="fInitialMomentumX"                                                \
     code="{fInitialMomentumX = onfile.fInitialMomentum.X();}"
#pragma read sourceClass="ND::TG4Trajectory" version="[-3]"        \
     source="TLorentzVector fInitialMomentum"                                 \
     targetClass="ND::TG4Trajectory"                               \
     target="fInitialMomentumY"                                                \
     code="{fInitialMomentumY = onfile.fInitialMomentum.Y();}"
#pragma read sourceClass="ND::TG4Trajectory" version="[-3]"        \
     source="TLorentzVector fInitialMomentum"                                 \
     targetClass="ND::TG4Trajectory"  target="fInitialMomentumZ"          \
     code="{fInitialMomentumZ = onfile.fInitialMomentum.Z();}"
#pragma read sourceClass="ND::TG4Trajectory" version="[-3]"        \
     source="TLorentzVector fInitialMomentum"                                 \
     targetClass="ND::TG4Trajectory"                               \
     target="fInitialMomentumT"                                                \
     code="{fInitialMomentumT = onfile.fInitialMomentum.T();}"
#endif
