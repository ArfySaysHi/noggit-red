#pragma once

#include "glm/ext/vector_float3.hpp"
#include "noggit/WmoLiquid.hpp"
#include <cstdint>

namespace WMOData {

union GroupFlags {
  uint32_t value;
  struct {
    uint32_t has_bsp_tree : 1;     // 0x1
    uint32_t has_light_map : 1;    // 0x2
    uint32_t has_vertex_color : 1; // 0x4
    uint32_t exterior : 1;         // 0x8
    uint32_t flag_0x10 : 1;
    uint32_t flag_0x20 : 1;
    uint32_t exterior_lit : 1; // 0x40
    uint32_t unreacheable : 1; // 0x80
    uint32_t flag_0x100 : 1;
    uint32_t has_light : 1; // 0x200
    uint32_t flag_0x400 : 1;
    uint32_t has_doodads : 1; // 0x800
    uint32_t has_water : 1;   // 0x1000
    uint32_t indoor : 1;      // 0x2000
    uint32_t flag_0x4000 : 1;
    uint32_t flag_0x8000 : 1;
    uint32_t always_draw : 1;   // 0x10000
    uint32_t has_mori_morb : 1; // 0x20000, cata+ only (?)
    uint32_t skybox : 1;        // 0x40000
    uint32_t ocean : 1;         // 0x80000
    uint32_t flag_0x100000 : 1;
    uint32_t mount_allowed : 1; // 0x200000
    uint32_t flag_0x400000 : 1;
    uint32_t flag_0x800000 : 1;
    uint32_t use_mocv2_for_texture_blending : 1; // 0x1000000
    uint32_t has_two_motv : 1;                   // 0x2000000
    uint32_t antiportal : 1;                     // 0x4000000
    uint32_t unk : 1; // 0x8000000 requires intBatchCount == 0, extBatchCount ==
                      // 0, UNREACHABLE.
    uint32_t unused : 4;
  };
};
static_assert(sizeof(GroupFlags) == sizeof(std::uint32_t),
              "bitfields shall be implemented packed");

struct GroupHeader {
  uint32_t group_name;             // offset into MOGN
  uint32_t descriptive_group_name; // offset into MOGN
  GroupFlags flags;
  float box1[3];
  float box2[3];
  uint16_t portal_start;
  uint16_t portal_count;
  uint16_t transparency_batches_count;
  uint16_t interior_batch_count;
  uint16_t exterior_batch_count;
  uint16_t padding_or_batch_type_d; // probably padding, but might be data?
  uint8_t fogs[4];
  uint32_t group_liquid; // used for MLIQ
  uint32_t id;
  int32_t unk2, unk3;
};

struct Batch {
  int8_t unused[12];

  uint32_t index_start;
  uint16_t index_count;
  uint16_t vertex_start;
  uint16_t vertex_end;

  uint8_t flags;
  uint8_t texture;
};

union MopyFlags {
  int8_t value;
  struct {
    int8_t flag_0x01 : 1;      // 0x1
    int8_t no_cam_collide : 1; // 0x2
    int8_t detail : 1;         // 0x4
    int8_t collision : 1;      // 0x8
    int8_t hint : 1;
    int8_t render : 1;
    int8_t flag_0x40 : 1;   // 0x40
    int8_t collide_hit : 1; // 0x80
  };
};
static_assert(sizeof(MopyFlags) == sizeof(std::int8_t),
              "bitfields shall be implemented packed");

struct TriangleMaterialInfo {
  MopyFlags flags;
  uint8_t texture;
};

enum MobnFlags {
  Flag_XAxis = 0x0,
  Flag_YAxis = 0x1,
  Flag_ZAxis = 0x2,
  Flag_AxisMask = 0x3,
  Flag_Leaf = 0x4,
  Flag_NoChild = 0xFFFF,
};

struct BspNode {
  uint16_t flags;
  int16_t negChild; // index of bsp child node (right in this array)
  int16_t posChild;
  uint16_t nFaces;    // num of triangle faces in MOBR
  uint32_t faceStart; // index of the first triangle index(in MOBR)
  float planeDist;
};

// Portal vertices list
struct Mopv {
  glm::vec3 a, b, c, d;
};

// Map object portal references from groups
struct Mopr {
  int16_t portal, group, dir, reserved;
};

struct DoodadSet {
  char name[0x14];
  int32_t start;
  int32_t size;
  int32_t unused;
};

union MohdFlags {
  std::uint16_t flags;
  struct {
    std::uint16_t do_not_attenuate_vertices_based_on_distance_to_portal : 1;
    std::uint16_t use_unified_render_path : 1;
    std::uint16_t use_liquid_type_dbc_id : 1;
    std::uint16_t do_not_fix_vertex_color_alpha : 1;
    std::uint16_t unused : 12;
  };
};
static_assert(sizeof(MohdFlags) == sizeof(std::uint16_t),
              "bitfields shall be implemented packed");

struct Header {
  uint32_t nTextures;
  uint32_t nGroups;
  uint32_t nPortals;
  uint32_t nLights;
  uint32_t nModels;
  uint32_t nDoodads;
  uint32_t nDoodadSets;
  uint32_t ambient_color;
  uint32_t wmoId;
  float extents[2][3];
  MohdFlags flags;
  uint16_t padding;
};

struct Material {
  union {
    uint32_t value;
    struct {
      uint32_t unlit : 1;
      uint32_t unfogged : 1;
      uint32_t unculled : 1;
      uint32_t ext_light : 1;
      uint32_t sidn : 1;
      uint32_t window : 1;
      uint32_t clamp_s : 1;
      uint32_t clamp_t : 1;
      uint32_t unused : 24;
    };
  } flags; // 0x00 (4 bytes)

  uint32_t shader;           // 0x04 (4 bytes)
  uint32_t blend_mode;       // 0x08 (4 bytes)
  uint32_t texture_offset_1; // 0x0C (4 bytes)

  CImVector sidn_color; // 0x10 (4 bytes) - Emissive color

  CImVector frame_sidn_color; // 0x14 (4 bytes) - Runtime value (must be in
                              // struct to align file read)

  uint32_t texture_offset_2; // 0x18 (4 bytes)
  CArgb diffuse_color;       // 0x1C (4 bytes)
  uint32_t ground_type;      // 0x20 (4 bytes)
  uint32_t texture_offset_3; // 0x24 (4 bytes)
  uint32_t color_2;          // 0x28 (4 bytes)
  uint32_t flag_2;           // 0x2C (4 bytes)

  uint32_t runtime_data[4];
};
static_assert(sizeof(WMOData::Material) == 0x40,
              "Material struct size mismatch");

struct Light {
  char type[4];
  uint32_t color;
  glm::vec3 pos;
  float intensity;
  float unkn[5];
  float r;
};

struct DoodadInstanceData {
  union {
    uint32_t value;
    struct {
      uint32_t name_offset : 24;       // Offset into MODN string table
      uint32_t flag_AcceptProjTex : 1; // 0x01
      uint32_t flag_0x2 : 1;           // 0x02 (Interior lighting vs Exterior)
      uint32_t flag_0x4 : 1;
      uint32_t flag_0x8 : 1;
      uint32_t unused : 4; // Padding
    };
  };

  float scale;          // Scale factor
  float orientation[4]; // x, y, z, w
  float position[3];
  uint32_t color_packed; // B, G, R, A
};
static_assert(sizeof(DoodadInstanceData) == 40,
              "DoodadInstanceData must be 40 bytes");
} // namespace WMOData
