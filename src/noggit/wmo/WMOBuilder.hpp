#pragma once

#include "noggit/data/WMOData.hpp"
class WMO;
class WMOParser;

class WMOBuilder {
public:
  explicit WMOBuilder(WMO *wmo);

  void buildFromParsedData(WMOData::Header header,
                           std::vector<char> textureBuffer,
                           std::vector<WMOData::Material> materials,
                           std::vector<WMOData::Light> lights,
                           std::vector<WMOData::DoodadInstanceData> doodads,
                           WMOData::GroupNameTable modelNames,
                           std::vector<WMOData::GroupHeader> groupHeaders,
                           WMOData::GroupNameTable groupNameTable);

private:
  WMO *_wmo;
};
