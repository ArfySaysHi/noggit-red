#include "noggit/data/WMOData.hpp"
#include <noggit/wmo/components/WMOMaterials.hpp>

namespace Noggit::WMO {

std::vector<WMOMaterial> WMOMaterials::buildFromRawData(
    const std::vector<WMOData::Material> &rawMaterials,
    const std::map<std::uint32_t, std::uint32_t> &texture_offset_to_index) {
  _materials.clear();
  _materials.reserve(_materials.size());

  for (const auto &raw_mat : rawMaterials) {
    WMOMaterial mat;
    *static_cast<WMOData::Material *>(&mat) = raw_mat;

    auto tex1_it = texture_offset_to_index.find(raw_mat.texture_offset_1);
    mat.texture1_index =
        (tex1_it != texture_offset_to_index.end()) ? tex1_it->second : 0;

    // TODO: Remove magic numbers... again...
    bool use_second_texture =
        (raw_mat.shader == 6 || raw_mat.shader == 5 || raw_mat.shader == 3);

    if (use_second_texture) {
      auto tex2_it = texture_offset_to_index.find(raw_mat.texture_offset_2);
      mat.texture2_index =
          (tex2_it != texture_offset_to_index.end()) ? tex2_it->second : 0;
    } else {
      mat.texture2_index = 0;
    }

    _materials.push_back(std::move(mat));
  }

  return _materials;
}

} // namespace Noggit::WMO
