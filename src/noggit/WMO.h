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
#include <noggit/WMOLighting.h>
#include <noggit/WmoLiquid.hpp>
#include <noggit/data/WMOData.hpp>
#include <noggit/rendering/Primitives.hpp>
#include <noggit/rendering/WMOGroupRender.hpp>
#include <noggit/rendering/WMORender.hpp>
#include <optional>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class WMO;
class WMOGroup;
class WMOInstance;
class WMOManager;
class wmo_liquid;
class Model;

namespace Noggit::Rendering {
class WMOGroupRender;
class WMORender;
} // namespace Noggit::Rendering

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
  std::map<uint32_t, std::vector<wmo_doodad_instance>>
  doodads_per_group(uint16_t doodadset) const;

  std::vector<WMOGroup> groups;
  std::vector<WMOMaterial> materials;
  glm::vec3 extents[2];
  std::vector<scoped_blp_texture_reference> textures;
  std::vector<std::string> models;
  std::vector<wmo_doodad_instance> modelis;
  std::vector<glm::vec3> model_nearest_light_vector;

  std::vector<WMOLight> lights;
  glm::vec4 ambient_light_color;

  uint32_t WmoId;

  WMOData::MohdFlags flags;

  std::vector<WMOFog> fogs;

  std::vector<WMOData::DoodadSet> doodadsets;

  std::optional<scoped_model_reference> skybox;

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

  Noggit::Rendering::WMORender _renderer;
};

class WMOManager {
public:
  static void report();
  static void clear_hidden_wmos();
  static void unload_all(Noggit::NoggitRenderContext context);

private:
  friend struct scoped_wmo_reference;
  static Noggit::AsyncObjectMultimap<WMO> _;
};

struct scoped_wmo_reference {
  scoped_wmo_reference(BlizzardArchive::Listfile::FileKey const &file_key,
                       Noggit::NoggitRenderContext context)
      : _valid(true), _file_key(file_key), _context(context),
        _wmo(WMOManager::_.emplace(file_key, context)) {}

  scoped_wmo_reference(scoped_wmo_reference const &other)
      : _valid(other._valid), _file_key(other._file_key),
        _wmo(WMOManager::_.emplace(_file_key, other._context)),
        _context(other._context) {}
  scoped_wmo_reference &operator=(scoped_wmo_reference const &other) {
    _valid = other._valid;
    _file_key = other._file_key;
    _wmo = WMOManager::_.emplace(_file_key, other._context);
    _context = other._context;
    return *this;
  }

  scoped_wmo_reference(scoped_wmo_reference &&other)
      : _valid(other._valid), _file_key(other._file_key), _wmo(other._wmo),
        _context(other._context) {
    other._valid = false;
  }
  scoped_wmo_reference &operator=(scoped_wmo_reference &&other) {
    std::swap(_valid, other._valid);
    std::swap(_file_key, other._file_key);
    std::swap(_wmo, other._wmo);
    std::swap(_context, other._context);
    other._valid = false;
    return *this;
  }

  ~scoped_wmo_reference() {
    if (_valid) {
      WMOManager::_.erase(_file_key, _context);
    }
  }

  WMO *operator->() const { return _wmo; }

  [[nodiscard]]
  WMO *get() const {
    return _wmo;
  }

private:
  bool _valid;

  BlizzardArchive::Listfile::FileKey _file_key;
  WMO *_wmo;
  Noggit::NoggitRenderContext _context;
};
