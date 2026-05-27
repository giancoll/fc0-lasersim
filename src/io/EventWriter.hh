#pragma once
#include <string>
#include <vector>

#include "io/EventData.hh"
#include "config/Config.hh"

#include <TVector3.h>

class EventWriter {
public:
    virtual ~EventWriter() = default;

    virtual void Initialize(const std::string& filename, int outputLevel,
                            const Config& cfg) = 0;

    /// truePositions / trueDirections: beam geometry points (origin, mirror hit, exit).
    virtual void WriteEvent(const ClusterEventData&  data, std::vector<TVector3> truePositions, std::vector<TVector3> trueDirections) = 0;
    virtual void WriteEvent(const AnodeEventData&    data, std::vector<TVector3> truePositions, std::vector<TVector3> trueDirections) = 0;
    virtual void WriteEvent(const WaveformEventData& data, std::vector<TVector3> truePositions, std::vector<TVector3> trueDirections) = 0;

    virtual void Finalize() = 0;
    virtual bool IsInitialized() const = 0;
};
