#pragma once

#include "ClientFile.hpp"
#include <noggit/data/WMOData.hpp>
#include <string>

struct ParsedMaterials {
  std::vector<WMOData::Material> materials;
  std::vector<std::string> texture_names;
};

class WMOParser {
public:
  WMOData::Header parseHeader(BlizzardArchive::ClientFile &file);
  std::vector<char> parseMOTX(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::Material> parseMOMT(BlizzardArchive::ClientFile &f);

private:
  WMOData::Header parseMOHD(BlizzardArchive::ClientFile &f);
};
