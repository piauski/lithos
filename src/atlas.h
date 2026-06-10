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
    b32 initialized;
} Atlas;

void atlas_init(const char *path, u16 rows, u16 cols);
void atlas_destroy();
const Atlas *atlas_get(void);

void atlas_get_uvs(const Atlas *a, Atlas_Index index, f32 *u0, f32 *u1, f32 *v0, f32 *v1);

#endif // !ATLAS_H_
