#pragma once

#include <noggit/data/WMOData.hpp>
#include <noggit/wmo/WMOLighting.hpp>
#include <vector>

namespace Noggit::WMO {

class WMOLights {
public:
  WMOLights() = default;

  std::vector<WMOLight>
  buildFromRawData(const std::vector<WMOData::Light> &rawLights);

  [[nodiscard]]
  size_t count() const {
    return _lights.size();
  }

  [[nodiscard]]
  const WMOLight &at(size_t index) const {
    return _lights.at(index);
  }

  [[nodiscard]]
  const std::vector<WMOLight> &getLights() const {
    return _lights;
  }

private:
  std::vector<WMOLight> _lights;
};

} // namespace Noggit::WMO
