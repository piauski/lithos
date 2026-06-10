#ifndef BLOCK_H_
#define BLOCK_H_

#include "atlas.h"

#include "core.h"

typedef enum {
    BLOCK_FACE_POS_X, // East
    BLOCK_FACE_NEG_X, // West
    BLOCK_FACE_POS_Y, // Top
    BLOCK_FACE_NEG_Y, // Bottom
    BLOCK_FACE_POS_Z, // South
    BLOCK_FACE_NEG_Z, // North
    __count_block_face,
} Block_Face;

typedef enum {
    BLOCK_AIR = 0,
    BLOCK_STONE,
    BLOCK_STONE_SLAB,
    BLOCK_TORCH,
    BLOCK_DIRT,
    BLOCK_GRASS,
    __count_block_kind,
} Block_Kind;

typedef struct {
    Vector3 from, to;
} Volume;

enum Atlas_Index;

typedef enum {
    BLOCK_FLAG_SOLID    = 1 << 0,
    BLOCK_FLAG_OCCLUDES = 1 << 1,
    BLOCK_FLAG_SIMPLE_TEXTURES = 1 << 2,
} Block_Flags;

typedef struct {
    const char *name;
    u32 flags;
    Volume volume;
    Atlas_Index textures[6];
} Block;

static const Volume volume_cube_all = {
    .from = { 0, 0, 0 },
    .to   = { 16, 16, 16 }
};

static const Volume volume_slab_bottom = {
    .from = { 0, 0, 0 },
    .to   = { 16, 8, 16 }
};

extern const Block block_definitions[__count_block_kind];

static inline bool block_is_solid(Block_Kind kind)
{
    return block_definitions[kind].flags & BLOCK_FLAG_SOLID;
}

static inline bool block_occludes(Block_Kind kind)
{
    return block_definitions[kind].flags & BLOCK_FLAG_OCCLUDES;
}

static inline const Atlas_Index *block_get_textures(Block_Kind kind)
{
    return block_definitions[kind].textures;
}


#endif // !BLOCK_H_
