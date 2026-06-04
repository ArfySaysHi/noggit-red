#pragma once

#include "noggit/data/WMOData.hpp"

struct WMOMaterial : public WMOData::Material {
  uint32_t texture1_index = 0;
  uint32_t texture2_index = 0;
};
