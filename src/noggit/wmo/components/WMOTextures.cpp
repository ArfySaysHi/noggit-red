#include <noggit/wmo/components/WMOTextures.hpp>

namespace Noggit::WMO {
WMOTextures::WMOTextures(NoggitRenderContext context) : _context(context) {}

std::vector<scoped_blp_texture_reference> WMOTextures::buildFromMaterials(
    const std::vector<WMOData::Material> &rawMaterials,
    const std::vector<char> &texbuf) {
  _textures.reserve(rawMaterials.size());

  for (const auto &raw_mat : rawMaterials) {
    loadOrGetTexture(raw_mat.texture_offset_1, texbuf);

    // TODO: Remove magic numbers here
    bool use_second_texture =
        (raw_mat.shader == 6 || raw_mat.shader == 5 || raw_mat.shader == 3);
    if (use_second_texture) {
      loadOrGetTexture(raw_mat.texture_offset_2, texbuf);
    }
  }

  return _textures;
}

std::uint32_t WMOTextures::loadOrGetTexture(std::uint32_t ofs,
                                            const std::vector<char> &texbuf) {
  auto mapping = _texture_offset_to_index.find(ofs);
  if (mapping != _texture_offset_to_index.end()) {
    return mapping->second;
  }

  const char *texture_path = (ofs < texbuf.size() && texbuf[ofs] != 0)
                                 ? &texbuf[ofs]
                                 : "textures/shanecube.blp";

  _textures.emplace_back(texture_path, _context);
  std::uint32_t new_index = static_cast<std::uint32_t>(_textures.size() - 1);

  _texture_offset_to_index[ofs] = new_index;
  return new_index;
}
} // namespace Noggit::WMO
