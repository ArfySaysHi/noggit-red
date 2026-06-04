#include "noggit/wmo/components/WMOLights.hpp"
#include <vector>

namespace Noggit::WMO {

std::vector<WMOLight>
WMOLights::buildFromRawData(const std::vector<WMOData::Light> &rawLights) {
  _lights.clear();
  _lights.reserve(rawLights.size());

  for (const auto &raw : rawLights) {
    WMOLight light;
    light.init(raw);
    _lights.push_back(light);
  }

  return _lights;
}

} // namespace Noggit::WMO
