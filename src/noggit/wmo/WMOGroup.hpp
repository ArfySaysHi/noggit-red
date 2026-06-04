#pragma once

#include <ClientFile.hpp>
#include <math/ray.hpp>
#include <noggit/ToolEnums.hpp>
#include <noggit/data/WMOData.hpp>
#include <noggit/rendering/WMOGroupRender.hpp>
#include <noggit/wmo/WMO.hpp>
#include <noggit/wmo/WMOLiquid.hpp>

namespace Noggit::WMO {

class WMOGroup {
public:
  WMOGroup(Noggit::WMO::WMO *wmo, const WMOData::GroupHeader &header,
           std::string name, int groupIndex);

  void load();

  void setupFog(bool draw_fog, std::function<void(bool)> setup_fog);

  bool intersect(math::ray const &ray, std::vector<float> *results) const;

  // todo: portal culling
  [[nodiscard]]
  bool is_visible(glm::mat4x4 const &transform_matrix,
                  math::frustum const &frustum, float const &cull_distance,
                  glm::vec3 const &camera, display_mode display) const;

  [[nodiscard]]
  std::vector<uint16_t> doodad_ref() const {
    return _doodad_ref;
  }

  glm::vec3 BoundingBoxMin;
  glm::vec3 BoundingBoxMax;
  glm::vec3 VertexBoxMin;
  glm::vec3 VertexBoxMax;

  bool use_outdoor_lights;
  std::string name;

  [[nodiscard]]
  bool has_skybox() const {
    return header.flags.skybox;
  }

  [[nodiscard]]
  bool is_indoor() const {
    return header.flags.indoor;
  }

  WMOData::GroupGeometry const &geometry() const { return _geometry; }

  [[nodiscard]]
  Noggit::Rendering::WMOGroupRender *renderer() {
    return &_renderer;
  }

  [[nodiscard]]
  Noggit::Rendering::WMOGroupRender const *renderer() const {
    return &_renderer;
  }

  ::glm::vec3 center;

private:
  void load_mocv(BlizzardArchive::ClientFile &f, uint32_t size);
  void fix_vertex_color_alpha(WMOData::GroupGeometry &geometry);

  Noggit::WMO::WMO *wmo;
  WMOData::GroupHeader header;
  float rad;
  int32_t num;
  int32_t fog;
  std::vector<uint16_t> _doodad_ref;
  std::unique_ptr<wmo_liquid> lq;

  std::vector<WMOData::TriangleMaterialInfo> _material_infos;

  WMOData::GroupGeometry _geometry;
  Noggit::Rendering::WMOGroupRender _renderer;
};

} // namespace Noggit::WMO
