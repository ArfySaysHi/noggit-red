#pragma once

#include "ClientFile.hpp"
#include <noggit/ModelManager.h>
#include <noggit/data/WMOData.hpp>

class WMOParser {
public:
  uint32_t parseMVER(BlizzardArchive::ClientFile &f);
  std::vector<char> parseMOTX(BlizzardArchive::ClientFile &f);
  WMOData::Header parseMOHD(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::Material> parseMOMT(BlizzardArchive::ClientFile &f);
  WMOData::GroupNameTable parseMOGN(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::GroupHeader> parseMOGI(BlizzardArchive::ClientFile &f,
                                              uint32_t nGroups);
  std::optional<WMOData::SkyboxData> parseMOSB(BlizzardArchive::ClientFile &f);
  void parseMOPV(BlizzardArchive::ClientFile &f);
  void parseMOPT(BlizzardArchive::ClientFile &f);
  void parseMOPR(BlizzardArchive::ClientFile &f);
  void parseMOVV(BlizzardArchive::ClientFile &f);
  void parseMOVB(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::Light> parseMOLT(BlizzardArchive::ClientFile &f,
                                        uint32_t nLights);
  std::vector<WMOData::DoodadSet> parseMODS(BlizzardArchive::ClientFile &f,
                                            uint32_t nDoodadSets);
  WMOData::GroupNameTable parseMODN(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::DoodadInstanceData>
  parseMODD(BlizzardArchive::ClientFile &f);
  std::vector<WMOData::Fog> parseMFOG(BlizzardArchive::ClientFile &f);
};
