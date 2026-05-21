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
