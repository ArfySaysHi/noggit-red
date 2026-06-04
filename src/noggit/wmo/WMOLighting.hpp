#pragma once

#include <ClientFile.hpp>
#include <noggit/data/WMOData.hpp>

#include <GL/gl.h>
#include <glm/glm.hpp>

struct WMOLight {
  glm::vec3 pos;
  glm::vec4 fcolor;
  float r;
  void init(const WMOData::Light &raw);
  void setup(GLint light);
  static void setupOnce(GLint light, glm::vec3 dir, glm::vec3 light_color);
};

struct WMOFog {
  glm::vec3 pos;
  float r1, r2;
  float fogend, fogstart;
  glm::vec4 color;
  void init(const WMOData::Fog &fog);
  void setup();
};
