#pragma once
#include "config/Config.hh"
#include "detector/GasModel.hh"

// Forward declarations
namespace Garfield {
    class Sensor;
    class Component;
    class SolidBox;
    class GeometrySimple;
}

/// Sets up the electric field and Garfield++ Sensor.
/// Supports multiple field configurations selected at runtime via Config:
///   - "user"     : ComponentUser with uniform analytical E/B field
///   - "constant" : ComponentConstant — uniform analytic field for quick tests
class FieldManager {
public:
    FieldManager() = default;
    ~FieldManager();

    void Initialize(const Config& cfg, GasModel& gas);

    Garfield::Sensor* GetSensor() { return m_sensor; }
    bool IsInitialized() const { return m_initialized; }

private:
    Garfield::Sensor*         m_sensor      = nullptr;
    Garfield::Component*      m_component   = nullptr;
    Garfield::Component*      m_componentB  = nullptr;
    Garfield::SolidBox*       m_box         = nullptr;
    Garfield::GeometrySimple* m_geometry    = nullptr;
    bool                      m_initialized = false;

    void SetupUserField    (const Config& cfg, GasModel& gas);
    void SetupConstantField(const Config& cfg, GasModel& gas);
    void BuildGeometry     (const Config& cfg, GasModel& gas);
};
