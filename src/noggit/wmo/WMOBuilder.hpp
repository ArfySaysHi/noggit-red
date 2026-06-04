#pragma once

#include "noggit/data/WMOData.hpp"
#include "noggit/wmo/components/WMODoodads.hpp"
#include "noggit/wmo/components/WMOLights.hpp"
#include "noggit/wmo/components/WMOMaterials.hpp"
#include "noggit/wmo/components/WMOTextures.hpp"
#include <memory>

class WMO;
class WMOParser;

class WMOBuilder {
public:
  explicit WMOBuilder(WMO *wmo, Noggit::NoggitRenderContext context);

  void buildFromParsedData(WMOData::Header header,
                           std::vector<char> textureBuffer,
                           std::vector<WMOData::Material> materials,
                           std::vector<WMOData::Light> lights,
                           std::vector<WMOData::DoodadInstanceData> doodads,
                           WMOData::GroupNameTable modelNames,
                           std::vector<WMOData::GroupHeader> groupHeaders,
                           WMOData::GroupNameTable groupNameTable);

  [[nodiscard]]
  const std::vector<scoped_blp_texture_reference> &getTextures() const {
    return _textures->getTextures();
  }

  [[nodiscard]]
  const std::vector<WMOMaterial> &getMaterials() const {
    return _materials->getMaterials();
  }

  [[nodiscard]]
  const std::vector<WMOLight> &getLights() const {
    return _lights->getLights();
  }

  [[nodiscard]]
  std::pair<std::vector<WMODoodadInstance>, std::vector<glm::vec3>>
  getDoodads() const {
    return {_doodads->getInstances(), _doodads->getNearestLightPositions()};
  }

private:
  WMO *_wmo;
  Noggit::NoggitRenderContext _context;

  std::unique_ptr<Noggit::WMO::WMOTextures> _textures;
  std::unique_ptr<Noggit::WMO::WMOMaterials> _materials;
  std::unique_ptr<Noggit::WMO::WMOLights> _lights;
  std::unique_ptr<Noggit::WMO::WMODoodads> _doodads;
};
