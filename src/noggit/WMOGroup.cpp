#include "noggit/data/WMOData.hpp"
#include <noggit/wmo/WMO.hpp>
#include <noggit/WMOGroup.hpp>
#include <noggit/application/NoggitApplication.hpp>

#include <iomanip>
#include <sstream>

WMOGroup::WMOGroup(WMO *_wmo, const WMOData::GroupHeader &header,
                   std::string name, int groupIndex)
    : wmo(_wmo), name(std::move(name)), num(groupIndex), _renderer() {
  VertexBoxMin = glm::vec3(header.box1[0], header.box1[1], header.box1[2]);
  VertexBoxMax = glm::vec3(header.box2[0], header.box2[1], header.box2[2]);
}

glm::vec4 colorFromInt(unsigned int col) {
  GLubyte r, g, b, a;
  a = (col & 0xFF000000) >> 24;
  r = (col & 0x00FF0000) >> 16;
  g = (col & 0x0000FF00) >> 8;
  b = (col & 0x000000FF);
  return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

// WMOGroup::WMOGroup(WMOGroup const &other)
//     : BoundingBoxMin(other.BoundingBoxMin),
//       BoundingBoxMax(other.BoundingBoxMax), VertexBoxMin(other.VertexBoxMin),
//       VertexBoxMax(other.VertexBoxMax),
//       use_outdoor_lights(other.use_outdoor_lights), name(other.name),
//     wmo(other.wmo), header(other.header), center(other.center),
//   rad(other.rad), num(other.num), fog(other.fog),
//_geometry(other._geometry), _doodad_ref(other._doodad_ref), _renderer() {
//  if (other.lq) {
//  lq = std::make_unique<wmo_liquid>(*other.lq.get());
//}
//}

void WMOGroup::load() {
  LogDebug << "WMOGroup::load() num=" << num
           << " wmo=" << wmo->file_key().filepath();
  std::stringstream curNum;
  curNum << "_" << std::setw(3) << std::setfill('0') << num;
  std::string fname = wmo->file_key().filepath();

  auto wmo_pos = fname.find(".wmo");
  if (wmo_pos == std::string::npos) {
    LogError << "WMOGroup::load(): could not find .wmo in " << fname;
    return;
  }

  fname.insert(fname.find(".wmo"), curNum.str());
  LogDebug << "WMOGroup::load() opening: " << fname;

  BlizzardArchive::ClientFile f(
      fname, Noggit::Application::NoggitApplication::instance()->clientData());
  if (f.isEof()) {
    LogError << "Error loading WMO \"" << fname << "\"." << std::endl;
    return;
  }

  uint32_t fourcc;
  uint32_t size;

  // - MVER ----------------------------------------------

  f.read(&fourcc, 4);
  f.seekRelative(4);

  uint32_t version;

  f.read(&version, 4);

  assert(fourcc == 'MVER' && version == 17);

  // - MOGP ----------------------------------------------

  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MOGP');

  f.read(&header, sizeof(WMOData::GroupHeader));

  unsigned fog_index = header.fogs[0];

  // downport hack
  if (fog_index >= wmo->fogs.size()) {
    fog_index = 0;
  }
  WMOFog &wf = wmo->fogs[fog_index];

  if (wf.r2 <= 0)
    fog = -1; // default outdoor fog..?
  else
    fog = header.fogs[0];

  BoundingBoxMin = ::glm::vec3(header.box1[0], header.box1[2], -header.box1[1]);
  BoundingBoxMax = ::glm::vec3(header.box2[0], header.box2[2], -header.box2[1]);

  // - MOPY ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPY');
  f.seekRelative(size);

  // - MOVI ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVI');

  _geometry.indices.resize(size / sizeof(uint16_t));

  f.read(_geometry.indices.data(), size);

  // - MOVT ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVT');

  // let's hope it's padded to 12 bytes, not 16...
  ::glm::vec3 const *vertices =
      reinterpret_cast<::glm::vec3 const *>(f.getPointer());

  VertexBoxMin = ::glm::vec3(std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max());
  VertexBoxMax = ::glm::vec3(std::numeric_limits<float>::lowest(),
                             std::numeric_limits<float>::lowest(),
                             std::numeric_limits<float>::lowest());

  rad = 0;

  _geometry.vertices.resize(size / sizeof(::glm::vec3));

  for (size_t i = 0; i < _geometry.vertices.size(); ++i) {
    _geometry.vertices[i] =
        glm::vec3(vertices[i].x, vertices[i].z, -vertices[i].y);

    ::glm::vec3 &v = _geometry.vertices[i];

    if (v.x < VertexBoxMin.x)
      VertexBoxMin.x = v.x;
    if (v.y < VertexBoxMin.y)
      VertexBoxMin.y = v.y;
    if (v.z < VertexBoxMin.z)
      VertexBoxMin.z = v.z;
    if (v.x > VertexBoxMax.x)
      VertexBoxMax.x = v.x;
    if (v.y > VertexBoxMax.y)
      VertexBoxMax.y = v.y;
    if (v.z > VertexBoxMax.z)
      VertexBoxMax.z = v.z;
  }

  center = (VertexBoxMax + VertexBoxMin) * 0.5f;
  rad = glm::length(VertexBoxMax - center) + 300.0f;

  f.seekRelative(size);

  // - MONR ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MONR');

  _geometry.normals.resize(size / sizeof(::glm::vec3));

  f.read(_geometry.normals.data(), size);

  for (auto &n : _geometry.normals) {
    n = {n.x, n.z, -n.y};
  }

  // - MOTV ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOTV');

  _geometry.texcoords.resize(size / sizeof(glm::vec2));

  f.read(_geometry.texcoords.data(), size);

  // - MOBA ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOBA');

  _geometry.batches.resize(size / sizeof(WMOData::Batch));
  f.read(_geometry.batches.data(), size);

  _renderer.initRenderBatches(_geometry, header.flags, wmo->materials);

  // - MOLR ----------------------------------------------
  if (header.flags.has_light) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOLR') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }
  }
  // - MODR ----------------------------------------------
  if (header.flags.has_doodads) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MODR') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      _doodad_ref.resize(size / sizeof(int16_t));
      f.read(_doodad_ref.data(), size);
    }
  }
  // - MOBN ----------------------------------------------
  if (header.flags.has_bsp_tree) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOBN') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }
  }
  // - MOBR ----------------------------------------------
  if (header.flags.has_bsp_tree) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOBR') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
      // std::vector<uint16_t> bsp_indices;
      // bsp_indices.resize(size / sizeof(uint16_t));
      // f.read(bsp_indices.data(), size);
      // _bsp_indices = bsp_indices;
    }
  }

  if (header.flags.flag_0x400) {
    // - MPBV ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MPBV') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }

    // - MPBP ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MPBP') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }

    // - MPBI ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MPBI') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }

    // - MPBG ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MPBG') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {

      f.seekRelative(size);
    }
  }
  // - MOCV ----------------------------------------------
  if (header.flags.has_vertex_color) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOCV') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      load_mocv(f, size);
    }
  }
  // - MLIQ ----------------------------------------------
  if (header.flags.has_water) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MLIQ') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      WMOLiquidHeader hlq;
      f.read(&hlq, 0x1E);

      lq = std::make_unique<wmo_liquid>(
          &f,
          hlq
          // , wmo->materials[hlq.material_id] // some models have mat_id = -1,
          // eg "world/wmo/dungeon/md_fishinghole/md_fishingholeice_001.wmo"
          ,
          header.group_liquid, (bool)wmo->flags.use_liquid_type_dbc_id,
          (bool)header.flags.ocean);

      // creating the wmo liquid doesn't move the position
      f.seekRelative(size - 0x1E);
    }
  }
  if (header.flags.has_mori_morb) {
    // - MORI ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MORI') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }

    // - MORB ----------------------------------------------
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MORB') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      f.seekRelative(size);
    }
  }

  // - MOTV ----------------------------------------------
  if (header.flags.has_two_motv) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOTV') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      _geometry.texcoords_2.resize(size / sizeof(glm::vec2));
      f.read(_geometry.texcoords_2.data(), size);
    }
  }
  // - MOCV ----------------------------------------------
  if (header.flags.use_mocv2_for_texture_blending) {
    f.read(&fourcc, 4);
    f.read(&size, 4);

    if (fourcc != 'MOCV') {
      LogError << "Broken header in WMO \"" << fname
               << "\". Trying to continue reading." << std::endl;
      f.seek(f.getPos() - 8);
    } else {
      std::vector<CImVector> mocv_2(size / sizeof(CImVector));
      f.read(mocv_2.data(), size);

      for (size_t i = 0; i < mocv_2.size(); ++i) {
        float alpha = static_cast<float>(mocv_2[i].a) / 255.f;

        // the second mocv is used for texture blending only
        if (header.flags.has_vertex_color) {
          _geometry.vertex_colors[i].w = alpha;
        } else // no vertex coloring, only texture blending with the alpha
        {
          _geometry.vertex_colors.emplace_back(0.f, 0.f, 0.f, alpha);
        }
      }
    }
  }

  // dl_light = 0;
  //  "real" lighting?
  if (header.flags.indoor && header.flags.has_vertex_color) {
    ::glm::vec3 dirmin(1, 1, 1);
    float lenmin;

    for (auto doodad : _doodad_ref) {
      if (doodad >= wmo->modelis.size()) {
        continue;
      }

      lenmin = 999999.0f * 999999.0f;
      ModelInstance &mi = wmo->modelis[doodad];
      for (unsigned int j = 0; j < wmo->lights.size(); j++) {
        WMOLight &l = wmo->lights[j];
        ::glm::vec3 dir = l.pos - mi.pos;

        float ll = glm::length(dir) * glm::length(dir);
        if (ll < lenmin) {
          lenmin = ll;
          dirmin = dir;
        }
      }
      wmo->model_nearest_light_vector[doodad] = dirmin;
    }

    use_outdoor_lights = false;
  } else {
    use_outdoor_lights = true;
  }
}

void WMOGroup::load_mocv(BlizzardArchive::ClientFile &f, uint32_t size) {
  uint32_t const *colors = reinterpret_cast<uint32_t const *>(f.getPointer());
  _geometry.vertex_colors.resize(size / sizeof(uint32_t));

  for (size_t i(0); i < size / sizeof(uint32_t); ++i) {
    _geometry.vertex_colors[i] = colorFromInt(colors[i]);
  }

  if (wmo->flags.do_not_fix_vertex_color_alpha) {
    int interior_batchs_start = 0;

    if (header.transparency_batches_count > 0) {
      interior_batchs_start =
          _geometry.batches[header.transparency_batches_count - 1].vertex_end +
          1;
    }

    for (int n = interior_batchs_start; n < _geometry.vertex_colors.size();
         ++n) {
      _geometry.vertex_colors[n].w = header.flags.exterior ? 1.f : 0.f;
    }
  } else {
    fix_vertex_color_alpha(_geometry);
  }

  // there's no read so this is required
  f.seekRelative(size);
}

bool WMOGroup::intersect(math::ray const &ray,
                         std::vector<float> *results) const {
  if (!ray.intersect_bounds(BoundingBoxMin, BoundingBoxMax)) {
    return false;
  }

  bool hit = false;

  for (std::size_t i = 0; i + 2 < _geometry.indices.size(); i += 3) {
    glm::vec3 const &v0 = _geometry.vertices[_geometry.indices[i + 0]];
    glm::vec3 const &v1 = _geometry.vertices[_geometry.indices[i + 1]];
    glm::vec3 const &v2 = _geometry.vertices[_geometry.indices[i + 2]];

    if (auto t = ray.intersect_triangle(v0, v1, v2)) {
      results->push_back(*t);
      hit = true;
    }
  }

  return hit;
}

bool WMOGroup::is_visible(glm::mat4x4 const &transform,
                          math::frustum const &frustum,
                          float const &cull_distance, glm::vec3 const &camera,
                          display_mode display) const {

  glm::vec3 corners[8] = {
      {BoundingBoxMin.x, BoundingBoxMin.y, BoundingBoxMin.z},
      {BoundingBoxMax.x, BoundingBoxMin.y, BoundingBoxMin.z},
      {BoundingBoxMin.x, BoundingBoxMax.y, BoundingBoxMin.z},
      {BoundingBoxMax.x, BoundingBoxMax.y, BoundingBoxMin.z},
      {BoundingBoxMin.x, BoundingBoxMin.y, BoundingBoxMax.z},
      {BoundingBoxMax.x, BoundingBoxMin.y, BoundingBoxMax.z},
      {BoundingBoxMin.x, BoundingBoxMax.y, BoundingBoxMax.z},
      {BoundingBoxMax.x, BoundingBoxMax.y, BoundingBoxMax.z},
  };

  glm::vec3 world_min(std::numeric_limits<float>::max());
  glm::vec3 world_max(std::numeric_limits<float>::lowest());

  for (auto const &corner : corners) {
    glm::vec3 w = glm::vec3(transform * glm::vec4(corner, 1.0f));
    world_min = glm::min(world_min, w);
    world_max = glm::max(world_max, w);
  }

  bool camera_inside = glm::all(glm::greaterThanEqual(camera, world_min)) &&
                       glm::all(glm::lessThanEqual(camera, world_max));

  if (!camera_inside && !frustum.intersects(world_min, world_max)) {
    return false;
  }

  // Use distance to the nearest point on the AABB
  if (!camera_inside) {
    glm::vec3 clamped = glm::clamp(camera, world_min, world_max);
    float dist = display == display_mode::in_3D
                     ? glm::distance(clamped, camera)
                     : std::abs(clamped.y - camera.y);
    if (dist >= cull_distance) {
      return false;
    }
  }

  return true;
}

/*
void WMOGroup::drawLiquid ( glm::mat4x4 const& transform
                          , liquid_render& render
                          , bool // draw_fog
                          , int animtime
                          )
{
  // draw liquid
  //! \todo  culling for liquid boundingbox or something
  if (lq)
  {
    gl.enable(GL_BLEND);
    gl.depthMask(GL_TRUE);

    lq->draw ( transform, render, animtime);

    gl.disable(GL_BLEND);
  }
}
*/

void WMOGroup::setupFog(bool draw_fog, std::function<void(bool)> setup_fog) {
  if (use_outdoor_lights || fog == -1) {
    setup_fog(draw_fog);
  } else {
    wmo->fogs[fog].setup();
  }
}
