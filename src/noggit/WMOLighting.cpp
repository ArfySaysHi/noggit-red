#include <noggit/WMOLighting.h>

#include <GL/gl.h>
#include <noggit/WMO.h>

void WMOLight::init(BlizzardArchive::ClientFile *f) {
  char type[4];
  f->read(&type, 4);
  f->read(&color, 4);
  f->read(&pos, 12);
  f->read(&intensity, 4);
  f->read(unk, 4 * 5);
  f->read(&r, 4);

  pos = glm::vec3(pos.x, pos.z, -pos.y);

  // rgb? bgr? hm
  float fa = ((color & 0xff000000) >> 24) / 255.0f;
  float fr = ((color & 0x00ff0000) >> 16) / 255.0f;
  float fg = ((color & 0x0000ff00) >> 8) / 255.0f;
  float fb = ((color & 0x000000ff)) / 255.0f;

  fcolor = glm::vec4(fr, fg, fb, fa);
  fcolor *= intensity;
  fcolor.w = 1.0f;

  /*
  // light logging
  gLog("Light %08x @ (%4.2f,%4.2f,%4.2f)\t %4.2f, %4.2f, %4.2f, %4.2f, %4.2f,
  %4.2f, %4.2f\t(%d,%d,%d,%d)\n", color, pos.x, pos.y, pos.z, intensity, unk[0],
  unk[1], unk[2], unk[3], unk[4], r, type[0], type[1], type[2], type[3]);
  */
}

void WMOLight::setup(GLint) {
  // not used right now -_-
}

void WMOLight::setupOnce(GLint, glm::vec3, glm::vec3) {
  // glm::vec4position(dir, 0);
  // glm::vec4position(0,1,0,0);

  // glm::vec4ambient = glm::vec4(light_color * 0.3f, 1);
  // glm::vec4diffuse = glm::vec4(light_color, 1);

  // gl.enable(light);
}

void WMOFog::init(BlizzardArchive::ClientFile *f) {
  f->read(this, 0x30);
  color = glm::vec4(((color1 & 0x00FF0000) >> 16) / 255.0f,
                    ((color1 & 0x0000FF00) >> 8) / 255.0f,
                    (color1 & 0x000000FF) / 255.0f,
                    ((color1 & 0xFF000000) >> 24) / 255.0f);
  float temp;
  temp = pos.y;
  pos.y = pos.z;
  pos.z = -temp;
  fogstart = fogstart * fogend * 1.5f;
  fogend *= 1.5;
}

void WMOFog::setup() {}

void WMOGroup::fix_vertex_color_alpha() {
  int interior_batchs_start = 0;

  if (header.transparency_batches_count > 0) {
    interior_batchs_start =
        _batches[header.transparency_batches_count - 1].vertex_end + 1;
  }

  glm::vec4 wmo_ambient_color;

  if (wmo->flags.use_unified_render_path) {
    wmo_ambient_color = {0.f, 0.f, 0.f, 0.f};
  } else {
    wmo_ambient_color = wmo->ambient_light_color;
    // w is not used, set it to 0 to avoid changing the vertex color alpha
    wmo_ambient_color.w = 0.f;
  }

  for (size_t i = 0; i < _vertex_colors.size(); ++i) {
    auto &color = _vertex_colors[i];
    float r = color.x;
    float g = color.y;
    float b = color.z;
    float a = color.w;

    // I removed the color = color/2 because it's just multiplied by 2 in the
    // shader afterward in blizzard's code
    if (i >= interior_batchs_start) {
      r += ((r * a / 64.f) - wmo_ambient_color.x);
      g += ((g * a / 64.f) - wmo_ambient_color.y);
      b += ((b * a / 64.f) - wmo_ambient_color.z);
    } else {
      r -= wmo_ambient_color.x;
      g -= wmo_ambient_color.y;
      b -= wmo_ambient_color.z;

      r = (r * (1.f - a));
      g = (g * (1.f - a));
      b = (b * (1.f - a));
    }

    color.x = std::min(255.f, std::max(0.f, r));
    color.y = std::min(255.f, std::max(0.f, g));
    color.z = std::min(255.f, std::max(0.f, b));
    color.w = 1.f; // default value used in the shader so I simplified it here,
                   // it can be overriden by the 2nd mocv chunk
  }
}
