#ifndef ATLAS_H_
#define ATLAS_H_

#include "core.h"

#include "raylib.h"

typedef enum {
    ATLAS_INDEX_STONE = 0,
    ATLAS_INDEX_DIRT,
    ATLAS_INDEX_GRASS_SIDE,
    ATLAS_INDEX_GRASS_TOP
} Atlas_Index;

typedef struct {
    u16 rows, cols;
    Texture2D texture;
} Atlas;

// TODO: Fix texture squishing on non-solid faces
void atlas_get_uvs(const Atlas *a, Atlas_Index index, f32 *u0, f32 *u1, f32 *v0, f32 *v1) {
    f32 u_step = 1.0f / a->cols;
    f32 v_step = 1.0f / a->rows;
    u16 x_index = index % a->cols;
    u16 y_index = index / a->cols;
    *u0 = (float)x_index * u_step;
    *v0 = (float)y_index * v_step;
    *u1 = (float)(x_index + 1) * u_step;
    *v1 = (float)(y_index + 1) * v_step;
}

#endif // !ATLAS_H_
