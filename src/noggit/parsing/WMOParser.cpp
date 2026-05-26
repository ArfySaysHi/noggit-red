#include <ClientFile.hpp>
#include <noggit/application/NoggitApplication.hpp>
#include <noggit/data/WMOData.hpp>
#include <noggit/parsing/WMOParser.hpp>
#include <optional>

uint32_t WMOParser::parseMVER(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc;
  uint32_t version;

  f.read(&fourcc, 4);
  f.seekRelative(4);
  f.read(&version, 4);

  assert(fourcc == 'MVER' && version == 17);

  return version;
}

WMOData::Header WMOParser::parseMOHD(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc;
  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MOHD');

  WMOData::Header header;
  f.read(&header, sizeof(WMOData::Header));

  return header;
}

static std::string GetStringFromBuffer(const std::vector<char> &buffer,
                                       uint32_t offset) {
  if (offset >= buffer.size())
    return "";
  return std::string(&buffer[offset]);
}

// Returns null-terminated texture names
std::vector<char> WMOParser::parseMOTX(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOTX');

  std::vector<char> texbuf(size);
  f.read(texbuf.data(), size);

  return texbuf;
};

std::vector<WMOData::Material>
WMOParser::parseMOMT(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOMT');

  size_t count = size / sizeof(WMOData::Material);

  std::vector<WMOData::Material> materials(count);
  if (count > 0) {
    f.read(materials.data(), size);
  }

  return materials;
}

WMOData::GroupNameTable WMOParser::parseMOGN(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOGN');

  WMOData::GroupNameTable table;
  table.buffer.resize(size);
  f.read(table.buffer.data(), size);

  return table;
}

std::vector<WMOData::GroupHeader>
WMOParser::parseMOGI(BlizzardArchive::ClientFile &f, uint32_t nGroups) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOGI');

  std::vector<WMOData::GroupHeader> headers(nGroups);
  if (nGroups > 0) {
    f.read(headers.data(), size);
  }

  return headers;
}

std::optional<WMOData::SkyboxData>
WMOParser::parseMOSB(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);
  assert(fourcc == 'MOSB');

  if (size > 4) {
    std::string path = BlizzardArchive::ClientData::normalizeFilenameInternal(
        std::string(reinterpret_cast<char const *>(f.getPointer())));

    static const std::string from = "mdx", to = "m2";
    for (size_t pos = 0; (pos = path.find(from, pos)) != std::string::npos;
         pos += to.size())
      path.replace(pos, from.size(), to);

    f.seekRelative(size);

    if (!path.empty())
      return WMOData::SkyboxData{std::move(path)};
  }

  f.seekRelative(size);
  return std::nullopt;
}

void WMOParser::parseMOPV(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPV');

  f.seekRelative(size);
}

void WMOParser::parseMOPT(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPT');

  f.seekRelative(size);
}

void WMOParser::parseMOPR(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOPR');

  f.seekRelative(size);
}

void WMOParser::parseMOVV(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVV');

  f.seekRelative(size);
}

void WMOParser::parseMOVB(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOVB');

  f.seekRelative(size);
}

std::vector<WMOData::Light> WMOParser::parseMOLT(BlizzardArchive::ClientFile &f,
                                                 uint32_t nLights) {
  uint32_t fourcc;
  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MOLT');

  std::vector<WMOData::Light> lights(nLights);
  if (nLights > 0) {
    f.read(lights.data(), sizeof(WMOData::Light) * nLights);
  }

  return lights;
}

std::vector<WMOData::DoodadSet>
WMOParser::parseMODS(BlizzardArchive::ClientFile &f, uint32_t nDoodadSets) {
  uint32_t fourcc;
  f.read(&fourcc, 4);
  f.seekRelative(4);

  assert(fourcc == 'MODS');

  std::vector<WMOData::DoodadSet> doodadSets(nDoodadSets);
  if (nDoodadSets > 0) {
    f.read(doodadSets.data(), sizeof(WMOData::DoodadSet) * nDoodadSets);
  }

  return doodadSets;
}

WMOData::GroupNameTable WMOParser::parseMODN(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MODN');

  WMOData::GroupNameTable table;
  table.buffer.resize(size);
  f.read(table.buffer.data(), size);

  return table;
}

std::vector<WMOData::DoodadInstanceData>
WMOParser::parseMODD(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MODD');

  size_t count = size / 40;
  std::vector<WMOData::DoodadInstanceData> instances(count);

  if (count > 0) {
    f.read(instances.data(), size);
  }

  return instances;
}

std::vector<WMOData::Fog> WMOParser::parseMFOG(BlizzardArchive::ClientFile &f) {
  uint32_t fourcc, size;
  f.read(&fourcc, 4);
  f.read(&size, 4);
  assert(fourcc == 'MFOG');

  size_t count = size / 48;
  std::vector<WMOData::Fog> fogs(count);

  if (count > 0) {
    f.read(fogs.data(), size);
  }

  return fogs;
}
