#include "chunk.h"
#include "core.h"

static const Vector3 face_vertices[6][4] = {
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}, // TOP
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}, // BOTTOM
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, // NORTH
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, // SOUTH
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}, // EAST
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}, // WEST
};

static const Vector3 face_normals[6] = {
	{0, 1, 0},  // TOP
	{0, -1, 0}, // BOTTOM
	{0, 0, -1}, // NORTH
	{0, 0, 1},  // SOUTH
	{1, 0, 0},  // EAST
	{-1, 0, 0}, // WEST
};

void chunk_free(Chunk *c)
{
    UnloadMesh(c->mesh);
}

static bool chunk_neighbor_is_transparent(Chunk *c, i32 x, i32 y, i32 z)
{
    // TODO: visit neighboring chunks
    if (x < 0 || x >= CHUNK_SIZE ||
        y < 0 || y >= CHUNK_SIZE ||
        z < 0 || z >= CHUNK_SIZE)
    {
        return true;
    }

    return block_is_transparent(chunk_get_block(c, x, y, z));
}

static u32 chunk_count_visible_faces(Chunk *c, i32 x, i32 y, i32 z)
{
    u32 faces = 0;
    if (chunk_neighbor_is_transparent(c, x + 1, y, z)) faces++;
    if (chunk_neighbor_is_transparent(c, x - 1, y, z)) faces++;
    if (chunk_neighbor_is_transparent(c, x, y + 1, z)) faces++;
    if (chunk_neighbor_is_transparent(c, x, y - 1, z)) faces++;
    if (chunk_neighbor_is_transparent(c, x, y, z + 1)) faces++;
    if (chunk_neighbor_is_transparent(c, x, y, z - 1)) faces++;
    return faces;
}

static bool chunk_face_visible(Chunk *c, i32 x, i32 y, i32 z, Block_Face face)
{
    Vector3 dir = face_normals[face];
    i32 nx = x + (i32)dir.x;
    i32 ny = y + (i32)dir.y;
    i32 nz = z + (i32)dir.z;
    return chunk_neighbor_is_transparent(c, nx, ny, nz);
}

static i32 chunk_add_face_from_volume(Mesh *mesh, Block_Face face, Volume volume, i32 x, i32 y, i32 z, i32 v)
{
    Vector3 n = face_normals[face];
    Vector3 min = Vector3Divide(volume.from, (Vector3){16.0f, 16.0f, 16.0f});
    Vector3 max = Vector3Divide(volume.to, (Vector3){16.0f, 16.0f, 16.0f});
    Vector3 a, b, c, d;
    switch (face) {
        case BLOCK_FACE_TOP: { // +y
            a = (Vector3){min.x, max.y, max.z};
            b = (Vector3){max.x, max.y, max.z};
            c = (Vector3){max.x, max.y, min.z};
            d = (Vector3){min.x, max.y, min.z};
        } break;
        case BLOCK_FACE_BOTTOM: { // -y
            a = (Vector3){min.x, min.y, min.z};
            b = (Vector3){max.x, min.y, min.z};
            c = (Vector3){max.x, min.y, max.z};
            d = (Vector3){min.x, min.y, max.z};
        } break;
        case BLOCK_FACE_NORTH: { // -z
            a = (Vector3){max.x, min.y, min.z};
            b = (Vector3){min.x, min.y, min.z};
            c = (Vector3){min.x, max.y, min.z};
            d = (Vector3){max.x, max.y, min.z};
        } break;
        case BLOCK_FACE_SOUTH: { // +z
            a = (Vector3){min.x, min.y, max.z};
            b = (Vector3){max.x, min.y, max.z};
            c = (Vector3){max.x, max.y, max.z};
            d = (Vector3){min.x, max.y, max.z};
        } break;
        case BLOCK_FACE_EAST: { // +x
            a = (Vector3){max.x, min.y, max.z};
            b = (Vector3){max.x, min.y, min.z};
            c = (Vector3){max.x, max.y, min.z};
            d = (Vector3){max.x, max.y, max.z};
        } break;
        case BLOCK_FACE_WEST: { // -x
            a = (Vector3){min.x, min.y, min.z};
            b = (Vector3){min.x, min.y, max.z};
            c = (Vector3){min.x, max.y, max.z};
            d = (Vector3){min.x, max.y, min.z};
        } break;
        default: assert(false && "Unreachable (Block_Face)");
    }

    Vector3 offset = (Vector3){ x, y, z };
    a = Vector3Add(a, offset);
    b = Vector3Add(b, offset);
    c = Vector3Add(c, offset);
    d = Vector3Add(d, offset);
    // Vector3 a = Vector3Add(face_vertices[face][0], offset);
    // Vector3 b = Vector3Add(face_vertices[face][1], offset);
    // Vector3 c = Vector3Add(face_vertices[face][2], offset);
    // Vector3 d = Vector3Add(face_vertices[face][3], offset);

    // triangle 1: a b c
    mesh->vertices[v*3+0] = a.x; mesh->vertices[v*3+1] = a.y; mesh->vertices[v*3+2] = a.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 0; mesh->texcoords[v*2+1] = 0;
    v++;

    mesh->vertices[v*3+0] = b.x; mesh->vertices[v*3+1] = b.y; mesh->vertices[v*3+2] = b.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 1; mesh->texcoords[v*2+1] = 0;
    v++;

    mesh->vertices[v*3+0] = c.x; mesh->vertices[v*3+1] = c.y; mesh->vertices[v*3+2] = c.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 1; mesh->texcoords[v*2+1] = 1;
    v++;

    // triangle 2: a c d
    mesh->vertices[v*3+0] = a.x; mesh->vertices[v*3+1] = a.y; mesh->vertices[v*3+2] = a.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 0; mesh->texcoords[v*2+1] = 0;
    v++;

    mesh->vertices[v*3+0] = c.x; mesh->vertices[v*3+1] = c.y; mesh->vertices[v*3+2] = c.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 1; mesh->texcoords[v*2+1] = 1;
    v++;

    mesh->vertices[v*3+0] = d.x; mesh->vertices[v*3+1] = d.y; mesh->vertices[v*3+2] = d.z;
    mesh->normals [v*3+0] = n.x; mesh->normals [v*3+1] = n.y; mesh->normals [v*3+2] = n.z;
    mesh->texcoords[v*2+0] = 0; mesh->texcoords[v*2+1] = 1;
    v++;
    return v;
}


void chunk_generate_mesh(Chunk *c)
{
    UnloadMesh(c->mesh);
    c->mesh = (Mesh){0};

    i32 faces = 0;
    for (u32 cx = 0; cx < CHUNK_SIZE; ++cx) {
        for (u32 cy = 0; cy < CHUNK_SIZE; ++cy) {
            for (u32 cz = 0; cz < CHUNK_SIZE; ++cz) {
                Block_Kind kind = chunk_get_block(c, cx, cy, cz);
                if (block_is_transparent(kind)) continue;
                faces += chunk_count_visible_faces(c, cx, cy, cz);
            }
        }
    }
    printf("Visible faces: %d\n", faces);

    i32 verts_per_face = 6;
    i32 total_verts = faces * verts_per_face;

    c->mesh.vertexCount = total_verts;
    c->mesh.triangleCount = faces * 2;

    c->mesh.vertices = (f32 *)MemAlloc(total_verts * 3 * sizeof(f32));
    c->mesh.normals = (f32 *)MemAlloc(total_verts * 3 * sizeof(f32));
    c->mesh.texcoords = (f32 *)MemAlloc(total_verts * 2 * sizeof(f32));


    i32 v = 0;
    for (u32 cx = 0; cx < CHUNK_SIZE; ++cx) {
        for (u32 cy = 0; cy < CHUNK_SIZE; ++cy) {
            for (u32 cz = 0; cz < CHUNK_SIZE; ++cz) {
                Block_Kind kind = chunk_get_block(c, cx, cy, cz);
                const Block *block = &block_definitions[kind];
                if (block_is_transparent(kind)) continue;
                for (Block_Face face = 0; face < __count_block_face; ++face) {
                    if (chunk_face_visible(c, cx, cy, cz, face)) {
                        v = chunk_add_face_from_volume(&c->mesh, face, block->volume, cx, cy, cz, v);
                    }
                }
            }
        }
    }

    UploadMesh(&c->mesh, false);
    c->model = LoadModelFromMesh(c->mesh);

    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);

    c->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    c->dirty = false;
}
