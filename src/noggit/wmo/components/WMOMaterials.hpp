#pragma once

#include "noggit/WMOMaterial.hpp"
#include "noggit/data/WMOData.hpp"
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

private:
  std::vector<WMOMaterial> _materials;
};

} // namespace Noggit::WMO
