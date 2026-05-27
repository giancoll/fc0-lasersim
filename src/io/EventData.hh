#pragma once
#include <vector>

struct ClusterEventData {
    int eventId    = 0;
    int nElectrons = 0;           // total primary electrons in this event

    std::vector<float> x, y, z;  // [mm] ionisation vertex
    std::vector<float> x_true, y_true, z_true; // [mm] true position before laser smearing (if applicable)
    std::vector<float> t;         // [ns] creation time
    std::vector<float> ekin;      // [eV] kinetic energy

    // TODO: add cl_clusterIdx (cluster index per electron) once the
    //       TrackHeed / LaserGenerator interface exposes cluster boundaries
};

struct AnodeEventData {
    int eventId    = 0;
    int nPrimaries = 0;           // electrons injected into drift
    int nAnodeHits = 0;           // electrons that reached the anode (status == 0)

    std::vector<float> z, y;     // [mm] hit position on anode plane (Z=left-right, Y=vertical)
    std::vector<float> t;         // [ns] arrival time
    std::vector<int>   status;   // Garfield++ drift status code (0 = OK, else lost)

    // Directly-hit pad index for each anode hit (before charge spreading to neighbours)
    std::vector<int>   eramId, padZ, padY;  // ERAM id, pad column (Z), pad row (Y)
};

struct WaveformEventData {
    int eventId     = 0;
    int nPrimaries  = 0;          // primary electrons in the event
    int nActivePads = 0;          // pads with signal
    int nSamples    = 0;          // ADC samples per waveform
    float samplePeriodNs = 0.0f;

    std::vector<int> eramId, padZ, padY;      // local pad address
    std::vector<int> peakSample, peakAdc;     // simple per-pad summary
    std::vector<int> adcStart, adcLength;     // slice of adc for each pad
    std::vector<int> adc;                     // concatenated ADC waveforms
};
