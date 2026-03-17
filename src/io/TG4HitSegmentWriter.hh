#pragma once
#include "TG4HitSegment.hxx"

/// Writable subclass of ND::TG4HitSegment.
///
/// TG4HitSegment exposes no public setters — all data members are protected.
/// Since they are *protected* (not private), a derived class can write them
/// directly.  This class provides a thin public setter API and can be used
/// anywhere an ND::TG4HitSegment* is expected.
///
class TG4HitSegmentWriter : public ND::TG4HitSegment {
public:
    void SetPrimaryId(int id)              { fPrimaryId       = id; }
    void SetEnergyDeposit(float e)         { fEnergyDeposit   = e; }
    void SetSecondaryDeposit(float e)      { fSecondaryDeposit = e; }
    void SetTrackLength(float l)           { fTrackLength     = l; }
    void AddContributor(int id)            { fContributors.push_back(id); }

    void SetStart(float x, float y, float z, float t) {
        fStartX = x; fStartY = y; fStartZ = z; fStartT = t;
    }
    void SetStop(float x, float y, float z, float t) {
        fStopX = x; fStopY = y; fStopZ = z; fStopT = t;
    }
};
