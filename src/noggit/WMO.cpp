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
#include <string>
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

  char const *ddnames = nullptr;
  char const *groupnames = nullptr;

  // - MVER ----------------------------------------------

  uint32_t version;

  f.read(&fourcc, 4);
  f.seekRelative(4);
  f.read(&version, 4);

  assert(fourcc == 'MVER' && version == 17);

  // - MOHD ----------------------------------------------

  WMOParser parser;
  WMOData::Header rawHeader = parser.parseHeader(f);

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

  // - MOTX ----------------------------------------------

  std::vector<char> texbuf = parser.parseMOTX(f);

  // - MOMT ----------------------------------------------

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

  // - MOGN ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOGN');

  groupnames = reinterpret_cast<char const *>(f.getPointer());

  f.seekRelative(size);

  // - MOGI ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOGI');

  groups.reserve(nGroups);
  for (int i(0); i < nGroups; ++i) {
    groups.emplace_back(this, &f, i, groupnames);
  }

  // - MOSB ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOSB');

  if (size > 4) {
    std::string path = BlizzardArchive::ClientData::normalizeFilenameInternal(
        std::string(reinterpret_cast<char const *>(f.getPointer())));
    auto from = std::string("mdx");
    auto to = std::string("m2");
    size_t start_pos = 0;
    while ((start_pos = path.find(from, start_pos)) != std::string::npos) {
      path.replace(start_pos, from.length(), to);
      start_pos +=
          to.length(); // Handles case where 'to' is a substring of 'from'
    }

    if (path.length()) {
      if (Noggit::Application::NoggitApplication::instance()
              ->clientData()
              ->exists(path)) {
        skybox = scoped_model_reference(path, _context);
      }
    }
  }

  f.seekRelative(size);

  // - MOPV ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPV');

  f.seekRelative(size);

  // - MOPT ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPT');

  f.seekRelative(size);

  // - MOPR ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPR');

  f.seekRelative(size);

  // - MOVV ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVV');

  f.seekRelative(size);

  // - MOVB ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVB');

  f.seekRelative(size);

  // - MOLT ----------------------------------------------

  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MOLT');

  lights.reserve(nLights);
  for (size_t i(0); i < nLights; ++i) {
    WMOLight l;
    l.init(&f);
    lights.push_back(l);
  }

  // - MODS ----------------------------------------------

  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MODS');

  doodadsets.reserve(nDoodadSets);
  for (size_t i(0); i < nDoodadSets; ++i) {
    WMOData::DoodadSet dds;
    f.read(&dds, 32);
    doodadsets.push_back(dds);
  }

  // - MODN ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MODN');

  if (size) {
    ddnames = reinterpret_cast<char const *>(f.getPointer());
    f.seekRelative(size);
  }

  // - MODD ----------------------------------------------

  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MODD');

  modelis.reserve(size / 0x28);
  for (size_t i(0); i < size / 0x28; ++i) {
    struct {
      uint32_t name_offset : 24;
      uint32_t flag_AcceptProjTex : 1;
      uint32_t flag_0x2 : 1;
      uint32_t flag_0x4 : 1;
      uint32_t flag_0x8 : 1;
      uint32_t flags_unused : 4;
    } x;

    size_t after_entry(f.getPos() + 0x28);
    f.read(&x, sizeof(x));

    modelis.emplace_back(ddnames + x.name_offset, &f, _context);
    model_nearest_light_vector.emplace_back();

    f.seek(after_entry);
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

std::map<uint32_t, std::vector<wmo_doodad_instance>>
WMO::doodads_per_group(uint16_t doodadset) const {
  std::map<uint32_t, std::vector<wmo_doodad_instance>> doodads;

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
