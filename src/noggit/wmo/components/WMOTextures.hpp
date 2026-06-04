#pragma once

#include "noggit/ContextObject.hpp"
#include "noggit/TextureManager.h"
#include "noggit/data/WMOData.hpp"
#include <cstdint>

namespace Noggit::WMO {
class WMOTextures {
public:
  explicit WMOTextures(Noggit::NoggitRenderContext context);

  std::vector<scoped_blp_texture_reference>
  buildFromMaterials(const std::vector<WMOData::Material> &rawMaterials,
                     const std::vector<char> &texbuf);

  [[nodiscard]]
  size_t count() const {
    return _textures.size();
  }

  [[nodiscard]]
  scoped_blp_texture_reference at(size_t index) const {
    return _textures.at(index);
  }

private:
  Noggit::NoggitRenderContext _context;
  std::vector<scoped_blp_texture_reference> _textures;
  std::map<std::uint32_t, std::uint32_t> _texture_offset_to_index;

  std::uint32_t loadOrGetTexture(std::uint32_t ofs,
                                 const std::vector<char> &texbuf);
};
} // namespace Noggit::WMO
