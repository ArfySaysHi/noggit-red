#pragma once

#include "noggit/data/WMOData.hpp"
#include "noggit/wmo/WMOMaterial.hpp"
#include <vector>

namespace Noggit::WMO {

class WMOMaterials {
public:
  WMOMaterials() = default;

  std::vector<WMOMaterial> buildFromRawData(
      const std::vector<WMOData::Material> &rawMaterials,
      const std::map<std::uint32_t, std::uint32_t> &texture_offset_to_index);

  [[nodiscard]]
  size_t count() const {
    return _materials.size();
  }

  [[nodiscard]]
  const std::vector<WMOMaterial> &getMaterials() const {
    return _materials;
  }

private:
  std::vector<WMOMaterial> _materials;
};

} // namespace Noggit::WMO
