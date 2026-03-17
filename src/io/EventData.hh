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
    std::vector<int>   padZ, padY;  // pad column (Z) and row (Y) indices
};

struct WaveformEventData {
    int eventId     = 0;
    int nPrimaries  = 0;          // primary electrons in the event
    int nActivePads = 0;          // pads with signal

    std::vector<int> padX, padY;              // pad column / row indices
    std::vector<std::vector<int>> adcWaveforms; // [pad][sample] ADC counts
};
