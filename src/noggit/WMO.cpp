// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include "noggit/data/WMOData.hpp"
#include <math/frustum.hpp>
#include <noggit/AsyncLoader.h>
#include <noggit/Log.h>            // LogDebug
#include <noggit/ModelManager.h>   // ModelManager
#include <noggit/TextureManager.h> // TextureManager, Texture
#include <noggit/WMO.h>
#include <noggit/WMOLighting.h>
#include <noggit/World.h>
#include <noggit/application/NoggitApplication.hpp>
#include <noggit/parsing/WMOParser.hpp>
#include <noggit/rendering/Primitives.hpp>
#include <opengl/scoped.hpp>

#include <algorithm>
#include <map>
#include <vector>

WMO::WMO(BlizzardArchive::Listfile::FileKey const &file_key,
         Noggit::NoggitRenderContext context)
    : AsyncObject(file_key), _context(context), _renderer(this) {}

void WMO::finishLoading() {
  BlizzardArchive::ClientFile f(
      _file_key.filepath(),
      Noggit::Application::NoggitApplication::instance()->clientData());
  if (f.isEof()) {
    LogError << "Error loading WMO \"" << _file_key.stringRepr() << "\"."
             << std::endl;
    return;
  }

  uint32_t fourcc;
  uint32_t size;

  float ff[3];

  char const *groupnames = nullptr;

  WMOParser parser;

  uint32_t version = parser.parseMVER(f);
  WMOData::Header rawHeader = parser.parseMOHD(f);
  _header = rawHeader;

  extents[0] = glm::vec3(_header.extents[0][0], _header.extents[0][1],
                         _header.extents[0][2]);
  extents[1] = glm::vec3(_header.extents[1][0], _header.extents[1][1],
                         _header.extents[1][2]);

  WmoId = _header.wmoId;
  flags = _header.flags;

  CArgb ambient_color = *reinterpret_cast<CArgb *>(&_header.ambient_color);
  ambient_light_color = glm::vec4(static_cast<float>(ambient_color.r) / 255.f,
                                  static_cast<float>(ambient_color.g) / 255.f,
                                  static_cast<float>(ambient_color.b) / 255.f,
                                  static_cast<float>(ambient_color.a) / 255.f);

  uint32_t nTextures = _header.nTextures;
  uint32_t nGroups = _header.nGroups;
  uint32_t nLights = _header.nLights;
  uint32_t nDoodadSets = _header.nDoodadSets;

  std::vector<char> texbuf = parser.parseMOTX(f);

  std::vector<WMOData::Material> rawMaterials = parser.parseMOMT(f);

  std::map<std::uint32_t, std::uint32_t> texture_offset_to_inmem_index;
  auto load_texture = [&](std::uint32_t ofs) {
    const char *texture_path = (ofs < texbuf.size() && texbuf[ofs] != 0)
                                   ? &texbuf[ofs]
                                   : "textures/shanecube.blp";

    auto mapping = texture_offset_to_inmem_index.find(ofs);
    if (mapping != texture_offset_to_inmem_index.end()) {
      return mapping->second;
    }

    textures.emplace_back(texture_path, _context);
    uint32_t new_index = static_cast<uint32_t>(textures.size() - 1);

    texture_offset_to_inmem_index[ofs] = new_index;
    return new_index;
  };

  materials.reserve(rawMaterials.size());
  for (const auto &raw_mat : rawMaterials) {
    WMOMaterial mat;
    *static_cast<WMOData::Material *>(&mat) = raw_mat;

    mat.texture1_index = load_texture(raw_mat.texture_offset_1);

    bool use_second_texture =
        (raw_mat.shader == 6 || raw_mat.shader == 5 || raw_mat.shader == 3);
    if (use_second_texture) {
      mat.texture2_index = load_texture(raw_mat.texture_offset_2);
    } else {
      mat.texture2_index = 0;
    }

    materials.push_back(std::move(mat));
  }

  groupnames = parser.parseMOGN(f);

  std::vector<WMOData::GroupHeader> rawGroupHeaders =
      parser.parseMOGI(f, _header.nGroups);

  groups.reserve(rawGroupHeaders.size());
  for (const auto &raw : rawGroupHeaders) {
    groups.emplace_back(this, raw, groupnames);
  }

  skybox = parser.parseMOSB(f, _context);
  parser.parseMOPV(f);
  parser.parseMOPT(f);
  parser.parseMOPR(f);
  parser.parseMOVV(f);
  parser.parseMOVB(f);

  std::vector<WMOData::Light> rawLights = parser.parseMOLT(f, nLights);

  lights.reserve(rawLights.size());
  for (const auto &raw : rawLights) {
    WMOLight l;
    l.init(raw);
    lights.push_back(l);
  }

  doodadsets = parser.parseMODS(f, nDoodadSets);
  char const *ddnames = parser.parseMODN(f);

  std::vector<WMOData::DoodadInstanceData> rawDoodadInstances =
      parser.parseMODD(f);

  modelis.reserve(rawDoodadInstances.size());
  model_nearest_light_vector.reserve(rawDoodadInstances.size());

  if (ddnames == nullptr && !rawDoodadInstances.empty()) {
    LogError << "MODN chunk missing or empty, but " << rawDoodadInstances.size()
             << " doodads found. Cannot resolve names.";
  }

  for (const auto &data : rawDoodadInstances) {
    if (ddnames == nullptr) {
      LogError << "Cannot load doodads: MODN names buffer is null";
      break;
    }

    const char *name = ddnames + data.name_offset;
    modelis.emplace_back(name, data, _context);
    model_nearest_light_vector.emplace_back();
  }

  // - MFOG ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MFOG');

  int nfogs = size / 0x30;
  fogs.reserve(nfogs);

  for (size_t i(0); i < nfogs; ++i) {
    WMOFog fog;
    fog.init(&f);
    fogs.push_back(std::move(fog));
  }

  for (auto &group : groups)
    group.load();

  finished = true;
  _state_changed.notify_all();
}

void WMO::waitForChildrenLoaded() {
  for (auto &tex : textures) {
    tex.get()->wait_until_loaded();
  }

  for (auto &doodad : modelis) {
    doodad.model->wait_until_loaded();
    doodad.model->waitForChildrenLoaded();
  }
}

std::vector<float> WMO::intersect(math::ray const &ray,
                                  bool do_exterior) const {
  std::vector<float> results;

  if (!finishedLoading() || loading_failed()) {
    return results;
  }

  for (auto &group : groups) {
    if (!do_exterior && !group.is_indoor())
      continue;

    group.intersect(ray, &results);
  }

  if (!do_exterior && results.size()) {
    // dirty way to find the furthest face and ignore invisible faces, cleaner
    // way would be to do a direction check on faces float max =
    // *std::max_element(std::begin(results), std::end(results));
    // results.clear();
    // results.push_back(max);

    // other way, ignore the closest intersect, works well
    if (results.size() > 1) {
      auto it = std::min_element(results.begin(), results.end());
      results.erase(it);
    }
  }

  return results;
}

std::map<uint32_t, std::vector<WMODoodadInstance>>
WMO::doodads_per_group(uint16_t doodadset) const {
  std::map<uint32_t, std::vector<WMODoodadInstance>> doodads;

  if (doodadset >= doodadsets.size()) {
    LogError << "Invalid doodadset for instance of wmo "
             << _file_key.stringRepr() << std::endl;
    return doodads;
  }

  auto const &dset = doodadsets[doodadset];
  uint32_t start = dset.start, end = start + dset.size;

  for (size_t i = 0; i < groups.size(); ++i) {
    for (uint16_t ref : groups[i].doodad_ref()) {
      if (ref >= start && ref < end) {
        doodads[i].push_back(modelis[ref]);
      }
    }
  }

  return doodads;
}
