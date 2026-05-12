// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <math/frustum.hpp>

#include <noggit/ToolEnums.hpp>

#include <opengl/scoped.hpp>
#include <opengl/shader.fwd.hpp>
#include <opengl/texture.hpp>

#include <QtGui/QImage>

#include <memory>

class MapIndex;
class MapTile;
class MapView;
class World;

namespace Noggit {

struct MapHorizon_tile {
  int16_t height_17[17][17];
  int16_t height_16[16][16];
  int16_t holes[16];
};

struct MapHorizon_batch {
  MapHorizon_batch() : vertex_start(0), vertex_count(0) {}

  MapHorizon_batch(uint32_t _vertex_start, uint32_t _vertex_count)
      : vertex_start(_vertex_start), vertex_count(_vertex_count) {}

  uint32_t vertex_start;
  uint32_t vertex_count;
};

class MapHorizon {
public:
  struct render {
    render(const MapHorizon &horizon);

    void draw(glm::mat4x4 const &model_view, glm::mat4x4 const &projection,
              MapIndex *index, const glm::vec3 &color,
              const float &cull_distance, const math::frustum &frustum,
              const glm::vec3 &camera, display_mode display);

    MapHorizon_batch _batches[64][64];

    OpenGL::Scoped::deferred_upload_vertex_arrays<1> _vaos;
    GLuint const &_vao = _vaos[0];
    OpenGL::Scoped::buffers<2> _buffers;
    GLuint const &_index_buffer = _buffers[0];
    GLuint const &_vertex_buffer = _buffers[1];
    std::unique_ptr<OpenGL::program> _MapHorizon_program;
  };

  class minimap : public OpenGL::texture {
  public:
    minimap(const MapHorizon &horizon);
  };

  MapHorizon(const std::string &basename, const MapIndex *const index);

  void update_minimap_tile(int y, int x, bool has_data);

  void set_minimap(const MapIndex *const index);

  void remove_horizon_tile(int y, int x);

  Noggit::MapHorizon_tile *get_horizon_tile(int y, int x);

  QImage _qt_minimap;

  void update_horizon_tile(MapTile *mTile);

  void save_wdl(World *world, bool regenerate = false);

private:
  int16_t getWdlheight(MapTile *tile, float x, float y);

  std::string _filename;

  std::vector<std::string> mWMOFilenames;
  // std::vector<ENTRY_MODF> lWMOInstances;

  std::unique_ptr<MapHorizon_tile> _tiles[64][64];
};

} // namespace Noggit
