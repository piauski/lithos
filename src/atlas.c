#include "atlas.h"
#include "raylib.h"
#include <assert.h>

static Atlas g_atlas_terrain;

void atlas_init(const char *path, u16 rows, u16 cols)
{
    if (g_atlas_terrain.initialized) return;

    Image image = LoadImage(path);
    g_atlas_terrain.texture = LoadTextureFromImage(image);
    UnloadImage(image);

    g_atlas_terrain.rows = rows;
    g_atlas_terrain.cols = cols;
    g_atlas_terrain.initialized = true;
}

void atlas_destroy()
{
    if (!g_atlas_terrain.initialized) return;

    UnloadTexture(g_atlas_terrain.texture);
    g_atlas_terrain.initialized = false;
}

const Atlas *atlas_get(void)
{
    assert(g_atlas_terrain.initialized && "Terrain Atlas is uninitialized!");
    return &g_atlas_terrain;
}

// TODO: Fix texture squishing on non-solid faces
void atlas_get_uvs(const Atlas *a, Atlas_Index index, f32 *u0, f32 *u1, f32 *v0, f32 *v1)
{
    f32 u_step = 1.0f / a->cols;
    f32 v_step = 1.0f / a->rows;
    u16 x_index = index % a->cols;
    u16 y_index = index / a->cols;
    *u0 = (float)x_index * u_step;
    *v0 = (float)y_index * v_step;
    *u1 = (float)(x_index + 1) * u_step;
    *v1 = (float)(y_index + 1) * v_step;
}
