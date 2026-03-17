#ifndef TTPCPadManager_hxx
#define TTPCPadManager_hxx

#include <cmath>

#include <TNamed.h>
#include <TVector3.h>

class TGeoNode;

namespace ND {
    class TTPCPadManager;
}

/// Provide low level interfaces to the TPC pad geometry data base.  The
/// TPCPadManager interface is used by oaGeomInfo which provides a higher
/// level (more friendly) interface.  The low level interface is primarily
/// used to store the TPC pad geometry inside of the event stream, or to
/// interface to external data bases.
///
/// This manager provides:
///   -# The number of columns
///   -# The number of rows
///   -# The pad center to center in local x (parallel to global Z)
///   -# The pad center to center in local y (parallel to global Y)
///   -# The gap between the pads (same in X and Y)
///   -# The x offset of the first pad.
///   -# The y offset of the first pad.
///
/// The access methods:
///   -# Channel Id to local coordinates (x,y)
///   -# Channel Id to global coordinates (X,Y,Z)
///   -# Pads in a row
///   -# Pads in a column
///   -# local coordinates (x,y) to channel id
///   -# global coordinates (X,Y,Z) to channel id
///   -# The MM module volume containing a channel id
///
/// All other information can be found using a combination of the methods
/// provided by this class and the ROOT geometry manager.  The oaGeomInfo
/// library provides the user interface.
/// 
/// Notes: 
/// - The pad plane is centered on the local MM module x, and y coordinates.  
/// - The pads are located at local -Z, so the drift distance is |Z_max| +
///   Z_local.
/// - Pad 0 is locate at local +X_max, +Y_max.
/// - The pad numbering runs down the rows (0 -> 47 goes +Y_max -> -Y_max) and
///   then over the columns (+X_max to -X_max).
///
class ND::TTPCPadManager: public TNamed {
public:
    TTPCPadManager();
    virtual ~TTPCPadManager();

    /// Convert a global channel number into a pad number in a module.  This
    /// will return -1 if the channel is not in the TPC.
    int ChannelToPad(int node, int key) const;

    /// Convert a global channel number into a module TGeoNode.  This will
    /// return NULL if the channel is not in the TPC.
    TGeoNode* ChannelToModuleNode(int node, int key) const;

    /// Convert a global channel number into a global channel position.  This
    /// will return false if the channel is not a TPC pad.
    bool ChannelToGlobalXYZ(int node, int key, TVector3& global) const;

    /// Convert a global position into a global channel number.  This will
    /// return false if the channel is not a TPC pad.
    bool GlobalXYZToChannel(const TVector3& global, int& node, int& key) const;

    /// Convert the pad number into a local X coordinate.
    double PadToLocalX(int pad) const {
        return ColumnToLocalX(PadToColumn(pad));
    }

    /// Convert the pad number into a local Y coordinate.
    double PadToLocalY(int pad) const {
        return RowToLocalY(PadToRow(pad));
    }

    /// Convert the column number into a local X coordinate
    double ColumnToLocalX(int column) const;

    /// Convert the row number into a local Y coordinate
    double RowToLocalY(int row) const;
    
    /// Convert a local X coordinate to the closest column number.
    int LocalXToColumn(double X) const;
    
    /// Convert a local Y coordinate to the closest row number.
    int LocalYToRow(double Y) const;

    /// Check if a local position is in a pad, allowing for the gap between
    /// pads.
    bool LocalXYInPad(double x, double y, int pad) const;

    /// Convert a row and column into a pad number.
    int RowAndColumnToPad(int row, int column) const;

    /// Convert a pad number to a row.
    int PadToRow(int pad) const;

    /// Convert a pad number to a column
    int PadToColumn(int pad) const;

    /// Get the number of rows.
    int GetPadRows() const {return fPadRows;}

    /// Get the number of columns.
    int GetPadColumns() const {return fPadColumns;}
    
    /// Get the number of pads.  Get the total number of pad locations on the
    /// micro-mega, including the inactive pads.  For instance, Pads 0 and 1
    /// are used for the voltage feed through, but are counted.
    int GetPadCount() const {return fPadRows*fPadColumns;}

    /// Get the X pitch (the horizontal pad center to center distance).
    double GetPadXPitch() const {return fPadXPitch;}

    /// Get the Y pitch (the vertical pad center to center distance).
    double GetPadYPitch() const {return fPadYPitch;}

    /// Get the gap between adjacent pads.
    double GetPadGap() const {return fPadGap;}

    /// Get the maximum local X.
    double GetLocalXMax() const {return fLocalXMax;}

    /// Get the maximum local Y.
    double GetLocalYMax() const {return fLocalYMax;}

    /// Get the minimum local X.
    double GetLocalXMin() const {
        return fLocalXMax - fPadXPitch*(fPadColumns-1);
    }

    /// Get the minimum local Y.
    double GetLocalYMin() const {
        return fLocalYMax - fPadYPitch*(fPadRows-1);
    }

    /// Provide setters for the data fields, but the default values should be
    /// OK.
    void SetPadRows(int rows);
    void SetPadColumns(int columns);
    void SetPadXPitch(double xPitch);
    void SetPadYPitch(double yPitch);
    void SetPadGap(double gap);
    void SetLocalXMax(double x);
    void SetLocalYMax(double y);

private:
    /// Make the copy constructor private.
    TTPCPadManager(const TTPCPadManager&) {MayNotUse("Copy Constructor");}

    /// The number of rows.
    int fPadRows;

    /// The number of columns.
    int fPadColumns;

    /// The center to center distance between pads in local X (global
    /// horizontal).
    double fPadXPitch;

    /// The center to center distance between pads in local Y (global
    /// vertical).
    double fPadYPitch;

    /// The gap between pads.
    double fPadGap;

    /// The maximum local X coordinate.
    double fLocalXMax;
    
    /// The maximum local Y coordinate.
    double fLocalYMax;
    
    ClassDef(TTPCPadManager,1);
};
#endif
