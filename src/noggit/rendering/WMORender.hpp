#pragma once

#include <glm/glm.hpp>
#include <math/frustum.hpp>
#include <noggit/ToolEnums.hpp>
#include <noggit/rendering/BaseRender.hpp>
#include <opengl/scoped.hpp>
#include <opengl/shader.hpp>

#include <map>

namespace Noggit::WMO {
class WMO;
}

namespace Noggit::Rendering {

class WMORender : public BaseRender {
public:
  WMORender(Noggit::WMO::WMO *wmoInstance) : _wmo(wmoInstance) {}

  void upload() override;
  void unload() override;

  void draw(OpenGL::Scoped::use_program &wmo_shader,
            glm::mat4x4 const &model_view, glm::mat4x4 const &projection,
            glm::mat4x4 const &transform, bool is_selected,
            math::frustum const &frustum, float const &cull_distance,
            glm::vec3 const &camera,
            std::map<int, std::pair<glm::vec3, glm::vec3>> const &group_extents,
            bool draw_doodads, bool draw_fog, int animtime,
            bool world_has_skies, display_mode display, bool interior_only);

  bool drawSkybox(glm::mat4x4 const &model_view, glm::vec3 const &camera_pos,
                  OpenGL::Scoped::use_program &m2_shader,
                  math::frustum const &frustum, const float &cull_distance,
                  int animtime, bool draw_particles, glm::vec3 aabb_min,
                  glm::vec3 aabb_max,
                  std::map<int, std::pair<glm::vec3, glm::vec3>> const
                      &group_extents) const;

  bool isUploaded() const;

private:
  Noggit::WMO::WMO *_wmo;
};

} // namespace Noggit::Rendering
