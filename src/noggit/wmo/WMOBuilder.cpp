#include <noggit/wmo/WMOBuilder.hpp>
#include <vector>

WMOBuilder::WMOBuilder(WMO *wmo) : _wmo(wmo) {};

void WMOBuilder::buildFromParsedData(
    WMOData::Header header, std::vector<char> textureBuffer,
    std::vector<WMOData::Material> materials,
    std::vector<WMOData::Light> lights,
    std::vector<WMOData::DoodadInstanceData> doodads,
    WMOData::GroupNameTable modelNames,
    std::vector<WMOData::GroupHeader> groupHeaders,
    WMOData::GroupNameTable groupNameTable) {

  this->buildMaterials(materials, textureBuffer);
  this->buildLights(lights);
  this->buildDoodads(doodads, modelNames);
};
