#pragma once

#include "noggit/ContextObject.hpp"
#include "noggit/ModelInstance.h"
#include "noggit/data/WMOData.hpp"
#include <vector>

namespace Noggit::WMO {

struct WMODoodadInstanceWithLight {
  WMODoodadInstance instance;
  glm::vec3 nearest_light_position;

  WMODoodadInstanceWithLight(WMODoodadInstance &&inst)
      : instance(std::move(inst)) {}
};

class WMODoodads {
public:
  explicit WMODoodads(Noggit::NoggitRenderContext context);

  std::pair<std::vector<WMODoodadInstance>, std::vector<glm::vec3>>
  buildFromRawData(const std::vector<WMOData::DoodadInstanceData> &rawInstances,
                   const WMOData::GroupNameTable &modelNames);

  [[nodiscard]]
  size_t count() const {
    return _instances.size();
  }

  [[nodiscard]]
  const std::vector<WMODoodadInstance> &getInstances() const {
    return _instances;
  }

  [[nodiscard]]
  const std::vector<glm::vec3> &getNearestLightPositions() const {
    return _nearest_light_positions;
  }

private:
  Noggit::NoggitRenderContext _context;
  std::vector<WMODoodadInstance> _instances;
  std::vector<glm::vec3> _nearest_light_positions;
};

} // namespace Noggit::WMO
