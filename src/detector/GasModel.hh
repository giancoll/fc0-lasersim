#pragma once
#include <string>
#include "config/Config.hh"

// Forward declaration
namespace Garfield { class MediumMagboltz; }

/// Manages the gas mixture and its properties via Garfield++/Magboltz.
/// Responsible for setting up or loading a pre-computed .gas file.
class GasModel {
public:
    GasModel() = default;
    ~GasModel();

    /// Set up the gas from config. If gas_file is non-empty and exists,
    /// loads from file; otherwise runs Magboltz to compute transport.
    void Initialize(const Config& cfg);

    /// Returns the underlying Garfield medium — owned by this class.
    Garfield::MediumMagboltz* GetMedium() { return m_medium; }

    bool IsInitialized() const { return m_initialized; }

private:
    Garfield::MediumMagboltz* m_medium     = nullptr;
    bool                       m_initialized = false;

    void ComputeWithMagboltz(const GasConfig& cfg);
    void LoadFromFile(const std::string& path, const GasConfig& cfg);
};
