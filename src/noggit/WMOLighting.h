#pragma once

#include "noggit/data/WMOData.hpp"
#include <ClientFile.hpp>

#include <GL/gl.h>
#include <cstdint>
#include <glm/glm.hpp>

struct WMOLight {
  uint32_t flags, color;
  glm::vec3 pos;
  float intensity;
  float unk[5];
  float r;
  glm::vec4 fcolor;

  void init(const WMOData::Light &raw);
  void setup(GLint light);
  static void setupOnce(GLint light, glm::vec3 dir, glm::vec3 light_color);
};

struct WMOFog {
  unsigned int flags;
  glm::vec3 pos;
  float r1, r2, fogend, fogstart;
  unsigned int color1;
  float f2;
  float f3;
  unsigned int color2;
  glm::vec4 color;

  void init(BlizzardArchive::ClientFile *f);
  void setup();
};
