// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include "noggit/data/WMOData.hpp"
#include "noggit/wmo/WMOBuilder.hpp"
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
    LogError << "Error loading WMO \"" << _file_key.stringRepr()
             << "\". isEof on open." << std::endl;
    finished = true;
    _state_changed.notify_all();
    return;
  }
  LogDebug << "WMO file opened ok: " << _file_key.stringRepr();

  WMOParser parser;

  parser.parseMVER(f);
  _header = parser.parseMOHD(f);

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

  auto texbuf = parser.parseMOTX(f);
  auto rawMaterials = parser.parseMOMT(f);
  auto groupNameTable = parser.parseMOGN(f);
  auto rawGroupHeaders = parser.parseMOGI(f, _header.nGroups);
  auto skyboxData = parser.parseMOSB(f);
  parser.parseMOPV(f);
  parser.parseMOPT(f);
  parser.parseMOPR(f);
  parser.parseMOVV(f);
  parser.parseMOVB(f);
  auto rawLights = parser.parseMOLT(f, _header.nLights);
  doodadsets = parser.parseMODS(f, _header.nDoodadSets);
  auto modelNames = parser.parseMODN(f);
  auto rawDoodads = parser.parseMODD(f);
  auto rawFogs = parser.parseMFOG(f);

  // TODO: Builder pattern for materials, lights and doodads

  groups.reserve(rawGroupHeaders.size());
  for (size_t i = 0; i < rawGroupHeaders.size(); ++i)
    groups.push_back(std::make_unique<WMOGroup>(
        this, rawGroupHeaders[i],
        groupNameTable.nameAt(rawGroupHeaders[i].group_name),
        static_cast<int>(i)));

  if (skyboxData) {
    auto *clientData =
        Noggit::Application::NoggitApplication::instance()->clientData();
    if (clientData->exists(skyboxData->path))
      skybox = ScopedModelReference(skyboxData->path, _context);
  }

  fogs.reserve(rawFogs.size());
  for (const auto &raw : rawFogs) {
    WMOFog fog;
    fog.init(raw);
    fogs.push_back(fog);
  }
  LogDebug << "Fogs ok, loading " << groups.size() << " groups";

  for (size_t i = 0; i < groups.size(); ++i) {
    LogDebug << "Loading group " << i;
    groups[i]->load();
    LogDebug << "Group " << i << " done";
  }
  LogDebug << "All groups loaded";

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
    if (!do_exterior && !group->is_indoor())
      continue;

    group->intersect(ray, &results);
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
    for (uint16_t ref : groups[i]->doodad_ref()) {
      if (ref >= start && ref < end) {
        doodads[i].push_back(modelis[ref]);
      }
    }
  }

  return doodads;
}
