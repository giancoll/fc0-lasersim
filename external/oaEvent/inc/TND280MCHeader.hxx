#ifndef TND280MCHeader_hxx_seen
#define TND280MCHeader_hxx_seen

#include "TDataVector.hxx"
#include "THandle.hxx"
#include "TSHAHashValue.hxx"

namespace ND {
    class TND280MCHeader;
}

/// Save information about how the MC was run.  This save information that
/// might be thought of as "MC Global Slow Control" like the magnetic field
/// used in the magnet.
class ND::TND280MCHeader: public ND::TDataVector {
public:
    TND280MCHeader();
    TND280MCHeader(const char* name, const char* title);
    virtual ~TND280MCHeader();

    /// Get the nominal magnetic field for the off-axis detector.  
    double GetOffAxisField() const {return fOffAxisField;}

    /// Set the magnetic field for the off-axis detector.
    void SetOffAxisField(double f) {fOffAxisField = f;}
         
    /// Get the magnetic field model type for the basket
    int GetBasketFieldModel() const {return fBasketFieldModel;}

    /// Set the magnetic field model type for the basket
    void SetBasketFieldModel(int b){fBasketFieldModel=b;}
              
    /// Get the magnetic field model type for the coil
    int GetCoilFieldModel() const {return fCoilFieldModel;}

    /// Set the magnetic field model type for the coil
    void SetCoilFieldModel(int b){fCoilFieldModel=b;}
           
    /// Get the magnetic field model type for the yoke
    int GetYokeFieldModel() const {return fYokeFieldModel;}

    /// Set the magnetic field model type for the yoke
    void SetYokeFieldModel(int b){fYokeFieldModel=b;};

    /// Set "Intensity" for the event.  For the beam spill MC, this should be
    /// the protons per pulse.
    void SetIntensity(double val) {fIntensity = val;}

    /// Get "Intensity" for the event.  For the beam spill MC, this should be
    /// the protons per pulse.
    double GetIntensity() const {return fIntensity;}

    /// Get the SHA hash value for the geometry used to generate the event.
    const TSHAHashValue& GetGeometryHash() const {return fGeometryHash;}

    /// Set the hash value for the geometry.
    void SetGeometryHash(const TSHAHashValue& h) {fGeometryHash = h;}
    
    /// Print the object information.
    virtual void ls(Option_t *opt = "") const;

private:

    /// The magnetic field used in the off-axis detector.  This is the
    /// "nominal" field, but there may be distortions that need to be applied. 
    float fOffAxisField;
    
    /// The magnetic field model type for the yoke, as defined in oaMagnetCalib.parameters.dat in
    /// package oaMagnetCalib
    int  fYokeFieldModel;
     
    /// The magnetic field model type for the basket, as defined in oaMagnetCalib.parameters.dat in
    /// package oaMagnetCalib
    int  fBasketFieldModel;
    
     
    /// The magnetic field model type for the coil, as defined in oaMagnetCalib.parameters.dat in
    /// package oaMagnetCalib
    int  fCoilFieldModel;


    /// The intensity used to generate this event.  The definition of the
    /// intensity can depend on the MC run type, but for a beam spill run, it
    /// will contain the number of protons per pulse. 
    float fIntensity;

    /// The hash value for the geometry that generated the event.
    TSHAHashValue fGeometryHash;

    ClassDef(TND280MCHeader,4);
};

#endif
