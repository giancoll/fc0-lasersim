#include <string>

#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TGeoBBox.h>

#include "HEPUnits.hxx"
#include "TOADatabase.hxx"
#include "THATPadManager.hxx"
#include "TND280Log.hxx"

ClassImp(ND::THATPadManager);

ND::THATPadManager::~THATPadManager() {}

ND::THATPadManager::THATPadManager()
    : TNamed("ND280HATPadManager", "HAT Pad Information"),
      // Note that here the HAT are rotated by 90 degrees compared with the normal TPCs.
      // This means that X and Y directions (along with the nomenclature column vs rows) are inter-changed.
      // The values are taken from the most recent presentation at ND280 Up meetings for now (MG 01/29/2020)
      fPadRows(32), fPadColumns(36),
      // this is not the pad size but the pad pitch (DOES include the gap between the pads)!!!!
      // 11.16 + 0.12 = 11.28
      // 10.07 + 0.12 = 10.19
      fPadXPitch(11.28 * unit::mm), fPadYPitch(10.19 * unit::mm),
      fPadGap(0.12 * unit::mm)
{
    fLocalXMax = 0.5 * (fPadXPitch * (fPadColumns - 1));
    fLocalYMax = 0.5 * (fPadYPitch * (fPadRows - 1));
}

int ND::THATPadManager::ChannelToPad(int, int key) const
{
    return key;
}

TGeoNode *ND::THATPadManager::ChannelToModuleNode(int node, int) const
{
    TGeoManager *geom = ND::TOADatabase::Get().Geometry();
    geom->PushPath();
    geom->CdNode(node);
    TGeoNode *moduleNode = geom->GetCurrentNode();
    geom->PopPath();
    std::string name(moduleNode->GetName());
    // ND280Log("Name: " << name);
    if (name.find("ERAM") == std::string::npos){
        ND280Log("Eram not found");
        return NULL;
    }
    return moduleNode;
}

bool ND::THATPadManager::ChannelToGlobalXYZ(int node, int key,
                                            TVector3 &global) const
{
    TGeoManager *geom = ND::TOADatabase::Get().Geometry();
    TGeoNode *moduleNode = ChannelToModuleNode(node, key);
    if (!moduleNode)
        return false;
    int pad = ChannelToPad(node, key);
    double localX = PadToLocalX(pad);
    double localY = PadToLocalY(pad);
    TGeoBBox *shape = dynamic_cast<TGeoBBox *>(moduleNode->GetVolume()->GetShape());
    double local[3] = {localX, localY, -(shape->GetDZ() - 0.01 * unit::mm)};
    double master[3] = {0, 0, 0};
    geom->PushPath();
    geom->CdNode(node);
    geom->LocalToMaster(local, master);
    geom->PopPath();
    global.SetXYZ(master[0], master[1], master[2]);
    return true;
}

bool ND::THATPadManager::GlobalXYZToChannel(const TVector3 &global,
                                            int &node, int &key) const
{
    node = -1;
    key = 0;
    TGeoManager *geom = ND::TOADatabase::Get().Geometry();
    // Find the node with this global position.
    geom->PushPath();
    TGeoNode *moduleNode = geom->FindNode(global.X(), global.Y(), global.Z());
    std::string moduleName(moduleNode->GetName());
    if (moduleName.find("ERAM_") == std::string::npos)
    {
        node = -1;
        geom->PopPath();
        return false;
    }
    // Make sure that the node is a micro-mega.
    node = geom->GetCurrentNodeId();
    // Conversion to local coordinates
    double master[3] = {global.X(), global.Y(), global.Z()};
    double local[3];
    geom->MasterToLocal(master, local);
    geom->PopPath();
    // Generate the local key (=pad) number.
    int row = LocalYToRow(local[1]);
    if (std::abs(local[1]) > fLocalYMax + 0.5*fPadYPitch)
        return false;
    int column = LocalXToColumn(local[0]);
    if (std::abs(local[0]) > fLocalXMax + 0.5*fPadXPitch)
        return false;
    key = RowAndColumnToPad(row, column);
    if (key < 0 || GetPadCount() <= key)
        return false;
    return true;
}

double ND::THATPadManager::ColumnToLocalX(int column) const
{
    return fLocalXMax - column * fPadXPitch;
}

double ND::THATPadManager::RowToLocalY(int row) const
{
    return fLocalYMax - row * fPadYPitch;
}

int ND::THATPadManager::LocalXToColumn(double X) const
{
    return int(0.5 + (fLocalXMax - X) / fPadXPitch);
}

int ND::THATPadManager::LocalYToRow(double Y) const
{
    return int(0.5 + (fLocalYMax - Y) / fPadYPitch);
}

bool ND::THATPadManager::LocalXYInPad(double x, double y, int pad) const
{
    return ((std::abs(x - PadToLocalX(pad)) < 0.5 * (fPadXPitch - fPadGap)) && (std::abs(y - PadToLocalY(pad)) < 0.5 * (fPadYPitch - fPadGap)));
}

int ND::THATPadManager::RowAndColumnToPad(int row, int column) const
{
    return row + column * fPadRows;
}

int ND::THATPadManager::PadToRow(int pad) const {
  unsigned int row = pad % fPadRows;
  return row;
//  return (row > 32) ? row : 32-row ;
}

int ND::THATPadManager::PadToColumn(int pad) const { return pad / fPadRows; }

void ND::THATPadManager::SetPadXPitch(double xPitch)
{
    fPadXPitch = xPitch;
    fLocalXMax = 0.5 * (fPadXPitch * (fPadColumns - 1));
}

void ND::THATPadManager::SetPadColumns(int columns)
{
    fPadColumns = columns;
    fLocalXMax = 0.5 * (fPadXPitch * (fPadColumns - 1));
}

void ND::THATPadManager::SetPadYPitch(double yPitch)
{
    fPadYPitch = yPitch;
    fLocalYMax = 0.5 * (fPadYPitch * (fPadRows - 1));
}

void ND::THATPadManager::SetPadRows(int rows)
{
    fPadRows = rows;
    fLocalYMax = 0.5 * (fPadYPitch * (fPadRows - 1));
}

void ND::THATPadManager::SetPadGap(double gap)
{
    fPadGap = gap;
}

void ND::THATPadManager::SetLocalXMax(double x)
{
    fLocalXMax = x;
}

void ND::THATPadManager::SetLocalYMax(double y)
{
    fLocalYMax = y;
}
