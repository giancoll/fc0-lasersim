#include "detector/FieldManager.hh"
#include <iostream>
#include <stdexcept>
#include <cmath>

#include <Garfield/Sensor.hh>
#include <Garfield/SolidBox.hh>
#include <Garfield/GeometrySimple.hh>
#include <Garfield/ComponentUser.hh>
#include <Garfield/ComponentConstant.hh>
#include <Garfield/MediumMagboltz.hh>

FieldManager::~FieldManager() {
    delete m_sensor;
    delete m_component;
    delete m_componentB;
    delete m_geometry;
    delete m_box;
}

void FieldManager::BuildGeometry(const Config& cfg, GasModel& gas) {
    const auto& det = cfg.detector;
    // SolidBox takes (center_x, center_y, center_z, half_lx, half_ly, half_lz) in cm
    const double cx = (det.tpcXminMm + det.tpcXmaxMm) / 2. / 10.;
    const double cy = (det.tpcYminMm + det.tpcYmaxMm) / 2. / 10.;
    const double cz = (det.tpcZminMm + det.tpcZmaxMm) / 2. / 10.;
    const double lx = (det.tpcXmaxMm - det.tpcXminMm) / 2. / 10.;
    const double ly = (det.tpcYmaxMm - det.tpcYminMm) / 2. / 10.;
    const double lz = (det.tpcZmaxMm - det.tpcZminMm) / 2. / 10.;
    m_box = new Garfield::SolidBox(cx, cy, cz, lx, ly, lz);

    m_geometry = new Garfield::GeometrySimple();
    m_geometry->AddSolid(m_box, gas.GetMedium());
}

void FieldManager::Initialize(const Config& cfg, GasModel& gas) {
    m_sensor = new Garfield::Sensor();

    const auto& type = cfg.run.fieldType;
    if      (type == "user")     SetupUserField    (cfg, gas);
    else if (type == "constant") SetupConstantField(cfg, gas);
    else
        throw std::runtime_error("FieldManager: unknown field_type: " + type);

    const auto& det = cfg.detector;
    m_sensor->SetArea(det.tpcXminMm / 10., det.tpcYminMm / 10., det.tpcZminMm / 10.,
                      det.tpcXmaxMm / 10., det.tpcYmaxMm / 10., det.tpcZmaxMm / 10.);

    std::cout << "[FieldManager] Sensor area: "
              << "x=[" << det.tpcXminMm << "," << det.tpcXmaxMm << "] mm, "
              << "y=[" << det.tpcYminMm << "," << det.tpcYmaxMm << "] mm, "
              << "z=[" << det.tpcZminMm << "," << det.tpcZmaxMm << "] mm\n";

    m_initialized = true;
    std::cout << "[FieldManager] Initialized with field type: " << type << "\n";
}

void FieldManager::SetupUserField(const Config& cfg, GasModel& gas) {
    BuildGeometry(cfg, gas);

    const double cathVolt = 27500.;
    const double efield   = cathVolt / (cfg.detector.driftLengthMm / 10.);
    const double bfield   = 0.0;

    auto* comp = new Garfield::ComponentUser();
    comp->SetGeometry(m_geometry);
    comp->SetElectricField(
        [efield](double, double, double, double& ex, double& ey, double& ez) {
            ex = efield; ey = 0.; ez = 0.;
        });
    comp->SetMagneticField(
        [bfield](double, double, double, double& bx, double& by, double& bz) {
            bx = 0.; by = 0.; bz = bfield;
        });

    m_sensor->AddComponent(comp);
    m_component = comp;
}

void FieldManager::SetupConstantField(const Config& cfg, GasModel& gas) {
    BuildGeometry(cfg, gas);

    const double cathVolt = 27500.;
    const double efield   = cathVolt / (cfg.detector.driftLengthMm / 10.);
    const double bfield   = 0.0;

    auto* comp = new Garfield::ComponentConstant();
    comp->SetGeometry(m_geometry);
    comp->SetElectricField(-efield, 0., 0.);
    comp->SetMagneticField(0.,      0., bfield);

    m_sensor->AddComponent(comp);
    m_component = comp;
}
