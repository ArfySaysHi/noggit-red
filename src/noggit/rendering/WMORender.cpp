#include "WMORender.hpp"
#include <map>
#include <noggit/wmo/WMO.hpp>

using namespace Noggit::Rendering;

void WMORender::upload() {
  for (auto &group : _wmo->groups) {
    group->renderer()->upload(group->geometry(), _wmo->materials,
                              _wmo->textures);
  }
}

void WMORender::unload() {
  for (auto &group : _wmo->groups) {
    group->renderer()->unload();
  }
}

void WMORender::draw(
    OpenGL::Scoped::use_program &wmo_shader, glm::mat4x4 const &model_view,
    glm::mat4x4 const &projection, glm::mat4x4 const &transform_matrix,
    bool boundingbox, math::frustum const &frustum, const float &cull_distance,
    const glm::vec3 &camera,
    std::map<int, std::pair<glm::vec3, glm::vec3>> const &group_extents,
    bool // draw_doodads
    ,
    bool draw_fog, int animtime, bool world_has_skies, display_mode display,
    bool interior_only) {

  if (!_wmo->finishedLoading()) [[unlikely]] {
    return;
  }

  // Check if this WMO has any indoor groups at all
  bool has_indoor_groups = std::any_of(
      _wmo->groups.begin(), _wmo->groups.end(),
      [](std::unique_ptr<WMOGroup> const &g) { return g->is_indoor(); });

  // If interior_only is requested but there are no indoor groups,
  // fall back to rendering all groups (pure exterior WMO)
  bool effective_interior_only = interior_only && has_indoor_groups;

  wmo_shader.uniform("ambient_color", glm::vec3(_wmo->ambient_light_color));

  for (int i = 0; i < (int)_wmo->groups.size(); ++i) {
    auto &group = _wmo->groups[i];

    // Restore original condition with corrected fallback
    if (effective_interior_only && !group->is_indoor())
      continue;

    auto it = group_extents.find(i);
    if (it == group_extents.end())
      continue;

    glm::vec3 const &group_min = it->second.first;
    glm::vec3 const &group_max = it->second.second;

    glm::vec3 clamped = glm::clamp(camera, group_min, group_max);
    float dist = glm::distance(clamped, camera);
    if (dist >= cull_distance)
      continue;

    group->renderer()->draw(wmo_shader, frustum, cull_distance, camera,
                            draw_fog, world_has_skies);
  }

  if (boundingbox) {
    for (int i = 0; i < (int)_wmo->groups.size(); ++i) {
      auto it = group_extents.find(i);
      if (it == group_extents.end())
        continue;

      Noggit::Rendering::Primitives::WireBox::getInstance(_wmo->_context)
          .draw(model_view, projection, glm::mat4x4(1.0f),
                {1.0f, 1.0f, 1.0f, 1.0f}, it->second.first, it->second.second);
    }

    Noggit::Rendering::Primitives::WireBox::getInstance(_wmo->_context)
        .draw(model_view, projection, transform_matrix,
              {1.0f, 0.0f, 0.0f, 1.0f},
              glm::vec3(_wmo->extents[0].x, _wmo->extents[0].z,
                        -_wmo->extents[0].y),
              glm::vec3(_wmo->extents[1].x, _wmo->extents[1].z,
                        -_wmo->extents[1].y));
  }
}

bool WMORender::drawSkybox(
    const glm::mat4x4 &model_view, const glm::vec3 &camera_pos,
    OpenGL::Scoped::use_program &m2_shader, const math::frustum &frustum,
    const float &cull_distance, int animtime, bool draw_particles,
    glm::vec3 aabb_min, glm::vec3 aabb_max,
    const std::map<int, std::pair<glm::vec3, glm::vec3>> &group_extents) const {
  if (!_wmo->skybox || !math::is_inside_of(camera_pos, aabb_min, aabb_max)) {
    return false;
  }

  for (int i = 0; i < _wmo->groups.size(); ++i) {
    auto const &g = _wmo->groups[i];

    if (!g->has_skybox()) {
      continue;
    }

    auto &extent(group_extents.at(i));

    if (math::is_inside_of(camera_pos, extent.first, extent.second)) {
      ModelInstance sky(_wmo->skybox.value()->file_key().filepath(),
                        _wmo->_context);
      sky.pos = camera_pos;
      sky.scale = 2.f;
      sky.recalcExtents();

      OpenGL::M2RenderState model_render_state;
      model_render_state.tex_arrays = {0, 0};
      model_render_state.tex_indices = {0, 0};
      model_render_state.tex_unit_lookups = {-1, -1};
      gl.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      gl.disable(GL_BLEND);
      gl.depthMask(GL_TRUE);
      m2_shader.uniform("blend_mode", 0);
      m2_shader.uniform("unfogged",
                        static_cast<int>(model_render_state.unfogged));
      m2_shader.uniform("unlit", static_cast<int>(model_render_state.unlit));
      m2_shader.uniform("tex_unit_lookup_1", 0);
      m2_shader.uniform("tex_unit_lookup_2", 0);
      m2_shader.uniform("pixel_shader", 0);

      _wmo->skybox->get()->renderer()->draw(
          model_view, sky, m2_shader, model_render_state, frustum,
          cull_distance, camera_pos, animtime, display_mode::in_3D);

      return true;
    }
  }

  return false;
}

bool WMORender::isUploaded() const {
  return std::all_of(_wmo->groups.begin(), _wmo->groups.end(),
                     [](std::unique_ptr<WMOGroup> const &g) {
                       return g->renderer()->isUploaded();
                     });
}
