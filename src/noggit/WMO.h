// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).
#pragma once
#include <math/ray.hpp>

#include <ClientFile.hpp>
#include <noggit/AsyncObjectMultimap.hpp>
#include <noggit/ContextObject.hpp>
#include <noggit/ModelInstance.h> // ModelInstance
#include <noggit/ModelManager.h>
#include <noggit/TextureManager.h>
#include <noggit/ToolEnums.hpp>
#include <noggit/WMOGroup.hpp>
#include <noggit/WMOLighting.h>
#include <noggit/WmoLiquid.hpp>
#include <noggit/data/WMOData.hpp>
#include <noggit/rendering/Primitives.hpp>
#include <noggit/rendering/WMOGroupRender.hpp>
#include <noggit/rendering/WMORender.hpp>
#include <optional>

#include <cstdint>
#include <string>
#include <vector>

class WMO;
class WMOInstance;
class wmo_liquid;
class Model;

namespace Noggit::Rendering {
class WMOGroupRender;
class WMORender;
} // namespace Noggit::Rendering

struct WMOMaterial : public WMOData::Material {
  uint32_t texture1_index = 0;
  uint32_t texture2_index = 0;
};

class WMO : public AsyncObject {
  friend class Noggit::Rendering::WMORender;

public:
  explicit WMO(BlizzardArchive::Listfile::FileKey const &file_key,
               Noggit::NoggitRenderContext context);

  [[nodiscard]]
  std::vector<float> intersect(math::ray const &,
                               bool do_exterior = true) const;

  void finishLoading() override;

  void waitForChildrenLoaded() override;

  [[nodiscard]]
  std::map<uint32_t, std::vector<WMODoodadInstance>>
  doodads_per_group(uint16_t doodadset) const;

  std::vector<WMOGroup> groups;
  std::vector<WMOMaterial> materials;
  glm::vec3 extents[2];
  std::vector<scoped_blp_texture_reference> textures;
  std::vector<std::string> models;
  std::vector<WMODoodadInstance> modelis;
  std::vector<glm::vec3> model_nearest_light_vector;

  std::vector<WMOLight> lights;
  glm::vec4 ambient_light_color;

  uint32_t WmoId;

  WMOData::MohdFlags flags;

  std::vector<WMOFog> fogs;

  std::vector<WMOData::DoodadSet> doodadsets;

  std::optional<ScopedModelReference> skybox;

  Noggit::NoggitRenderContext _context;

  [[nodiscard]]
  bool is_hidden() const {
    return _hidden;
  }

  void toggle_visibility() { _hidden = !_hidden; }
  void show() { _hidden = false; }
  void hide() { _hidden = true; }

  [[nodiscard]]
  bool is_required_when_saving() const override {
    return true;
  }

  [[nodiscard]]
  Noggit::Rendering::WMORender *renderer() {
    return &_renderer;
  }

private:
  bool _hidden = false;
  WMOData::Header _header;

  Noggit::Rendering::WMORender _renderer;
};
