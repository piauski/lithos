#ifndef CHUNK_H_
#define CHUNK_H_

#include "raylib.h"

#include "core.h"
#include "maths.h"
#include "block.h"

#define CHUNK_SIZE 16
#define CHUNK_IDX(x, y, z) (x + (CHUNK_SIZE) * (y + (CHUNK_SIZE) * z))

typedef struct {
    Vector3i position;
    u8 blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    bool dirty;
    Mesh mesh;
    Model model;
} Chunk;

void chunk_free(Chunk *c);
void chunk_generate_mesh(Chunk *c);

static inline void chunk_set_block(Chunk *c, int x, int y, int z, Block_Kind type)
{
    c->blocks[CHUNK_IDX(x, y, z)] = type;
    c->dirty = true;
}

static inline Block_Kind chunk_get_block(Chunk *c, i32 x, i32 y, i32 z)
{
    return c->blocks[CHUNK_IDX(x, y, z)];
}


#endif // !CHUNK_H_
