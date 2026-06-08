#ifndef BLOCK_H_
#define BLOCK_H_

#include "raymath.h"

#include "core.h"

typedef enum {
    BLOCK_FACE_TOP,
    BLOCK_FACE_BOTTOM,
    BLOCK_FACE_NORTH,
    BLOCK_FACE_SOUTH,
    BLOCK_FACE_EAST,
    BLOCK_FACE_WEST,
    __count_block_face,
} Block_Face;

typedef enum {
    BLOCK_AIR = 0,
    BLOCK_STONE,
    BLOCK_STONE_SLAB_BOTTOM,
    __count_block_type,
} Block_Kind;

typedef struct {
    Vector3 from, to;
} Volume;

typedef struct {
    Volume volume;
    b32 is_transparent;
    b32 is_solid;
} Block;

static const Volume volume_full_cube = {
    .from = { 0, 0, 0 },
    .to   = { 16, 16, 16 }
};

static const Volume volume_slab_bottom = {
    .from = { 0, 0, 0 },
    .to   = { 16, 8, 16 }
};

extern const Block block_definitions[__count_block_type];

static inline bool block_is_transparent(Block_Kind kind)
{
    return block_definitions[kind].is_transparent;
}


#endif // !BLOCK_H_
