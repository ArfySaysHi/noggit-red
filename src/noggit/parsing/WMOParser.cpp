#include "ClientFile.hpp"
#include "noggit/data/WMOData.hpp"
#include <noggit/parsing/WMOParser.hpp>

WMOData::Header WMOParser::parseHeader(BlizzardArchive::ClientFile &file) {
  return parseMOHD(file);
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
  uint32_t fourcc;
  uint32_t size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOTX');

  std::vector<char> texbuf(size);
  f.read(texbuf.data(), size);

  return texbuf;
};

std::vector<WMOData::Material>
WMOParser::parseMOMT(BlizzardArchive::ClientFile &f) {

  uint32_t fourcc;
  uint32_t size;
  f.read(&fourcc, 4);
  f.read(&size, 4);

  assert(fourcc == 'MOMT');

  size_t count = size / sizeof(WMOData::Material);

  std::vector<WMOData::Material> materials(count);
  if (count < 0) {
    f.read(materials.data(), size);
  }

  return materials;
}
