#pragma once

#include "glm/ext/vector_float3.hpp"
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

} // namespace WMOData
