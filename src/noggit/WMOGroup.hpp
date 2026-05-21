#pragma once

#include "math/ray.hpp"
#include "noggit/ToolEnums.hpp"
#include "noggit/WmoLiquid.hpp"
#include "noggit/data/WMOData.hpp"
#include <ClientFile.hpp>
#include <noggit/rendering/WMOGroupRender.hpp>

class WMO;

class WMOGroup {
  friend class Noggit::Rendering::WMOGroupRender;

public:
  WMOGroup(WMO *wmo, BlizzardArchive::ClientFile *f, int num,
           char const *names);
  WMOGroup(WMOGroup const &);

  void load();

  void setupFog(bool draw_fog, std::function<void(bool)> setup_fog);

  void intersect(math::ray const &, std::vector<float> *results) const;

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

  [[nodiscard]]
  Noggit::Rendering::WMOGroupRender *renderer() {
    return &_renderer;
  };
  ::glm::vec3 center;

private:
  void load_mocv(BlizzardArchive::ClientFile &f, uint32_t size);
  void fix_vertex_color_alpha();

  WMO *wmo;
  WMOData::GroupHeader header;
  float rad;
  int32_t num;
  int32_t fog;
  std::vector<uint16_t> _doodad_ref;
  std::unique_ptr<wmo_liquid> lq;

  std::vector<WMOData::TriangleMaterialInfo> _material_infos;
  std::vector<WMOData::Batch> _batches;

  std::vector<::glm::vec3> _vertices;
  std::vector<::glm::vec3> _normals;
  std::vector<glm::vec2> _texcoords;
  std::vector<glm::vec2> _texcoords_2;
  std::vector<glm::vec4> _vertex_colors;
  std::vector<uint16_t> _indices;

  std::optional<std::vector<WMOData::BspNode>> _bsp_tree_nodes;
  std::optional<std::vector<uint16_t>> _bsp_indices;

  Noggit::Rendering::WMOGroupRender _renderer;
};
