#include "detector/GasModel.hh"
#include <iostream>
#include <stdexcept>

#include <Garfield/MediumMagboltz.hh>

GasModel::~GasModel() {
    delete m_medium;
}

void GasModel::Initialize(const Config& cfg) {
    m_medium = new Garfield::MediumMagboltz();

    const GasConfig& gas = cfg.gas;
    if (!gas.gasFile.empty()) {
        LoadFromFile(gas.gasFile, gas);
    } else {
        ComputeWithMagboltz(gas);
    }

    m_initialized = true;
    std::cout << "[GasModel] Initialized: ";
    for (size_t i = 0; i < gas.components.size(); ++i)
        std::cout << gas.components[i] << " " << gas.fractions[i] << "% ";
    std::cout << "\n";
}

void GasModel::ComputeWithMagboltz(const GasConfig& gas) {
    const size_t n = std::min(gas.components.size(), gas.fractions.size());
    if (n == 0) throw std::runtime_error("GasModel: no gas components specified");
    if (n > 3)  throw std::runtime_error("GasModel: at most 3 gas components supported");

    if (n == 1)
        m_medium->SetComposition(gas.components[0], gas.fractions[0]);
    else if (n == 2)
        m_medium->SetComposition(gas.components[0], gas.fractions[0],
                                  gas.components[1], gas.fractions[1]);
    else
        m_medium->SetComposition(gas.components[0], gas.fractions[0],
                                  gas.components[1], gas.fractions[1],
                                  gas.components[2], gas.fractions[2]);

    m_medium->SetTemperature(gas.temperatureK);
    m_medium->SetPressure(gas.pressureAtm * 760.);

    m_medium->SetFieldGrid(10., 275., 5, /*useLog=*/true,
                            0., 0.3, 4,
                            0., 1.5, 5);
    m_medium->GenerateGasTable(10, /*verbose=*/true);
    std::cout << "[GasModel] Magboltz table generated.\n";

    if (!gas.saveGasFile.empty()) {
        m_medium->WriteGasFile(gas.saveGasFile);
        std::cout << "[GasModel] Gas table saved to: " << gas.saveGasFile << "\n";
    }
}

void GasModel::LoadFromFile(const std::string& path, const GasConfig& gas) {
    if (!m_medium->LoadGasFile(path))
        throw std::runtime_error("GasModel: cannot load gas file: " + path);

    m_medium->SetTemperature(gas.temperatureK);
    m_medium->SetPressure(gas.pressureAtm * 760.);
    std::cout << "[GasModel] Loaded gas file: " << path << "\n";
}
