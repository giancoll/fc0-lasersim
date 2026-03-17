#include <TClass.h>
#include <TStreamerInfo.h>

#include "HEPUnits.hxx"
#include "TG4HitSegment.hxx"

ClassImp(ND::TG4HitSegment);

ND::TG4HitSegment::TG4HitSegment()
    : fPrimaryId(0), fEnergyDeposit(0), fSecondaryDeposit(0), fTrackLength(0),
      fStartX(0), fStartY(0), fStartZ(0), fStartT(0),
      fStopX(0), fStopY(0), fStopZ(0), fStopT(0) {}

ND::TG4HitSegment::TG4HitSegment(const ND::TG4HitSegment& rhs)
    : TG4VHit(rhs),
      fContributors(rhs.fContributors), fPrimaryId(rhs.fPrimaryId),
      fEnergyDeposit(rhs.fEnergyDeposit),
      fSecondaryDeposit(rhs.fSecondaryDeposit),
      fTrackLength(rhs.fTrackLength),
      fStartX(rhs.fStartX), fStartY(rhs.fStartY),
      fStartZ(rhs.fStartZ), fStartT(rhs.fStartT),
      fStopX(rhs.fStopX), fStopY(rhs.fStopY),
      fStopZ(rhs.fStopZ), fStopT(rhs.fStopT) {}


ND::TG4HitSegment::~TG4HitSegment() {}

void ND::TG4HitSegment::ls(Option_t *opt) const {
    ND::ls_header(this,opt);
    std::cout << " Primary Id: " << GetPrimaryId();
    // Save the current precision so the modified values can be reset later.
    int prec = std::cout.precision();
    double energy = GetEnergyDeposit();
    std::cout.precision(1);
    if (energy > 1*unit::GeV) {
        std::cout << " E: " << energy/unit::GeV << " GeV";
    }
    else if (energy > 1*unit::MeV) {
        std::cout << " E: " << energy/unit::MeV << " MeV";
    }
    else {
        std::cout << " E: " << energy/unit::keV << " keV";
    }
    double secondary = GetSecondaryDeposit();
    if (secondary > 1*unit::GeV) {
        std::cout << " S: " << secondary/unit::GeV << " GeV";
    }
    else if (secondary > 1*unit::MeV) {
        std::cout << " S: " << secondary/unit::MeV << " MeV";
    }
    else {
        std::cout << " S: " << secondary/unit::keV << " keV";
    }
    std::cout << " L: " << GetTrackLength()/unit::mm << " mm" << std::endl;
    std::string option(opt);
    if (option.find("dump") != std::string::npos) {
        TROOT::IncreaseDirLevel();
        TROOT::IndentLevel();
        std::cout << std::setprecision(1)
                  << "Start: (" << fStartX << " cm"
                  << ", " << fStartY << " cm"
                  << ", " << fStartZ << " cm"
                  << std::setprecision(0)
                  << ", " << fStartT << " ns)"
                  << std::endl;
        TROOT::IndentLevel();
        std::cout << std::setprecision(1)
                  << "Stop:  (" << fStopX << " cm"
                  << ", " << fStopY << " cm"
                  << ", " << fStopZ << " cm"
                  << std::setprecision(0)
                  << ", " << fStopT << " ns)"
                  << std::endl;
        TROOT::IndentLevel();
        std::cout << "Traj. Ids:";
        for (std::vector<int>::const_iterator i = GetContributors().begin();
             i != GetContributors().end();
             ++i) std::cout << " " << *i;
        std::cout << std::endl;
        TROOT::DecreaseDirLevel();
    }
    std::cout.precision(prec);
}
