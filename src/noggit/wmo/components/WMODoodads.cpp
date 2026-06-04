#include "glm/ext/vector_float3.hpp"
#include "noggit/data/WMOData.hpp"
#include <noggit/wmo/components/WMODoodads.hpp>

namespace Noggit::WMO {

WMODoodads::WMODoodads(NoggitRenderContext context) : _context(context) {}

std::pair<std::vector<WMODoodadInstance>, std::vector<glm::vec3>>
WMODoodads::buildFromRawData(
    const std::vector<WMOData::DoodadInstanceData> &rawInstances,
    const WMOData::GroupNameTable &modelNames) {
  _instances.clear();
  _nearest_light_positions.clear();

  _instances.reserve(rawInstances.size());
  _nearest_light_positions.reserve(rawInstances.size());

  for (const auto &data : rawInstances) {
    std::string model_path = modelNames.nameAt(data.name_offset);
    _instances.emplace_back(model_path, data, _context);
    _nearest_light_positions.emplace_back();
  }

  return {_instances, _nearest_light_positions};
};

} // namespace Noggit::WMO
