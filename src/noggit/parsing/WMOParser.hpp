#pragma once

#include <noggit/data/WMOData.hpp>

#include "ClientFile.hpp"
class WMOParser {
public:
  WMOData::Header parseHeader(BlizzardArchive::ClientFile &file);

private:
  WMOData::Header parseMOHD(BlizzardArchive::ClientFile &f);
};
