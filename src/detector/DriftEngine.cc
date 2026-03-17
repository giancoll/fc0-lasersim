#include "detector/DriftEngine.hh"
#include <iostream>
#include <stdexcept>

#include <Garfield/AvalancheMicroscopic.hh>
#include <Garfield/AvalancheMC.hh>
#include <Garfield/DriftLineRKF.hh>

DriftEngine::~DriftEngine() {
    delete m_avalanche;
    delete m_avMC;
    delete m_rkf;
}

void DriftEngine::Initialize(Garfield::Sensor* sensor, const Config& cfg) {
    m_method = cfg.run.driftMethod;

    if (m_method == "RKF") {
        m_rkf = new Garfield::DriftLineRKF();
        m_rkf->SetSensor(sensor);

    } else if (m_method == "MC") {
        m_avMC = new Garfield::AvalancheMC();
        m_avMC->SetSensor(sensor);
        m_avMC->SetDistanceSteps(0.1); // 1 mm steps in Garfield++'s cm units

    } else if (m_method == "detailed") {
        // AvalancheMicroscopic: full microscopic tracking (slowest but most accurate)
        m_avalanche = new Garfield::AvalancheMicroscopic();
        m_avalanche->SetSensor(sensor);

    } else {
        throw std::runtime_error("DriftEngine: unknown drift_method: " + m_method);
    }

    m_initialized = true;
    std::cout << "[DriftEngine] Initialized with method: " << m_method << "\n";
}

// Input positions (x, y, z) are in mm; Garfield++ uses cm.
// Returned DriftPoint positions are converted back to mm.
std::vector<DriftPoint> DriftEngine::DriftElectron(double x, double y,
                                                    double z, double t) {
    std::vector<DriftPoint> endpoints;

    if (m_method == "RKF") {
        m_rkf->DriftElectron(x / 10., y / 10., z / 10., t);
        double xp, yp, zp, tp;
        int status = 0;
        m_rkf->GetEndPoint(xp, yp, zp, tp, status);
        endpoints.push_back({xp * 10., yp * 10., zp * 10., tp, status});

    } else if (m_method == "MC") {
        m_avMC->DriftElectron(x / 10., y / 10., z / 10., t);
        double xe1, ye1, ze1, te1, xe2, ye2, ze2, te2;
        int status = 0;
        m_avMC->GetElectronEndpoint(0,
                                     xe1, ye1, ze1, te1,
                                     xe2, ye2, ze2, te2,
                                     status);
        endpoints.push_back({xe2 * 10., ye2 * 10., ze2 * 10., te2, status});

    } else if (m_method == "detailed") {
        m_avalanche->AvalancheElectron(x / 10., y / 10., z / 10., t, 0., 0., 0., 0.);
        const unsigned int ne = m_avalanche->GetNumberOfElectronEndpoints();
        for (unsigned int i = 0; i < ne; ++i) {
            double x0, y0, z0, t0, e0;
            double x1, y1, z1, t1, e1;
            int status = 0;
            m_avalanche->GetElectronEndpoint(i,
                                              x0, y0, z0, t0, e0,
                                              x1, y1, z1, t1, e1,
                                              status);
            endpoints.push_back({x1 * 10., y1 * 10., z1 * 10., t1, status});
        }
    }

    return endpoints;
}
