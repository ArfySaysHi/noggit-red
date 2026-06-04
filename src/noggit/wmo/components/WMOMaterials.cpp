#include "noggit/data/WMOData.hpp"
#include <noggit/wmo/components/WMOMaterials.hpp>

namespace Noggit::WMO {

std::vector<WMOMaterial> WMOMaterials::buildFromRawData(
    const std::vector<WMOData::Material> &rawMaterials,
    const std::map<std::uint32_t, std::uint32_t> &texture_offset_to_index) {
  _materials.clear();
  _materials.reserve(rawMaterials.size());

  for (const auto &raw_mat : rawMaterials) {
    WMOMaterial mat;
    *static_cast<WMOData::Material *>(&mat) = raw_mat;

    auto tex1_it = texture_offset_to_index.find(raw_mat.texture_offset_1);
    mat.texture1_index =
        (tex1_it != texture_offset_to_index.end()) ? tex1_it->second : 0;

    assert(raw_mat.shader <
           static_cast<uint32_t>(WMOData::WMOWotLKShaderType::Count));
    auto shader_type = static_cast<WMOData::WMOWotLKShaderType>(raw_mat.shader);

    if (WMOData::usesSecondTexture(shader_type)) {
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
