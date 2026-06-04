#include <noggit/wmo/WMOBuilder.hpp>
#include <noggit/wmo/components/WMODoodads.hpp>
#include <noggit/wmo/components/WMOLights.hpp>
#include <noggit/wmo/components/WMOMaterials.hpp>
#include <noggit/wmo/components/WMOTextures.hpp>
#include <vector>

WMOBuilder::WMOBuilder(WMO *wmo, Noggit::NoggitRenderContext context)
    : _wmo(wmo), _context(context) {}

void WMOBuilder::buildFromParsedData(
    WMOData::Header header, std::vector<char> textureBuffer,
    std::vector<WMOData::Material> materials,
    std::vector<WMOData::Light> lights,
    std::vector<WMOData::DoodadInstanceData> doodads,
    WMOData::GroupNameTable modelNames,
    std::vector<WMOData::GroupHeader> groupHeaders,
    WMOData::GroupNameTable groupNameTable) {

  _textures = std::make_unique<Noggit::WMO::WMOTextures>(_context);
  _textures->buildFromMaterials(materials, textureBuffer);

  _materials = std::make_unique<Noggit::WMO::WMOMaterials>();
  _materials->buildFromRawData(materials, _textures->getOffsetToIndexMap());

  _lights = std::make_unique<Noggit::WMO::WMOLights>();
  _lights->buildFromRawData(lights);

  _doodads = std::make_unique<Noggit::WMO::WMODoodads>(_context);
  _doodads->buildFromRawData(doodads, modelNames);
};
