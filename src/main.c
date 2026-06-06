#include <stdbool.h>
#include "core.h"

#include "raylib.h"
#include "raymath.h"

#include "lith_math.h"

#define Vector3_Fmt "(X: %f Y: %f Z: %f)"
#define Vector3_Arg(vec) (vec).x, (vec).y, (vec).z

#define SCREEN_FACTOR 100
#define WIDTH 16 * SCREEN_FACTOR
#define HEIGHT 9 * SCREEN_FACTOR

#define CHUNK_SIZE 16
#define CHUNK_IDX(x, y, z) (x + (CHUNK_SIZE) * (y + (CHUNK_SIZE) * z))

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
    __count_block_type,
} Block_Kind;

typedef struct {
    Vector3 from, to;
} Volume;

typedef struct {
    Volume volume;
    b32 is_transparent;
} Block;

static const Volume volume_full_cube = {
    .from = { 0, 0, 0 },
    .to   = { 16, 16, 16 }
};

static const Block block_definitions[__count_block_type] = {
    [BLOCK_AIR] = { .is_transparent = true },
    [BLOCK_STONE] = {
        .volume = volume_full_cube,
    },
};

static inline bool block_is_transparent(Block_Kind kind)
{
    return block_definitions[kind].is_transparent;
}

typedef struct {
    Vector3i position;
    u8 blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    bool dirty;
    Mesh mesh;
    Model model;
} Chunk;

static Vector3 face_vertices[6][4] = {
    {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}, {0, 1, 0}}, // TOP
    {{0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1}}, // BOTTOM
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, // NORTH
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, // SOUTH
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}, // EAST
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}, // WEST
};

static Vector3 face_normals[6] = {
	{0, 1, 0},  // TOP
	{0, -1, 0}, // BOTTOM
	{0, 0, -1}, // NORTH
	{0, 0, 1},  // SOUTH
	{1, 0, 0},  // EAST
	{-1, 0, 0}, // WEST
};

i32 add_face(Mesh *mesh, Block_Face face, i32 x, i32 y, i32 z, i32 v)
{
    Vector3 n = face_normals[face];

    Vector3 offset = (Vector3){ x, y, z };

    Vector3 a = Vector3Add(face_vertices[face][0], offset);
    Vector3 b = Vector3Add(face_vertices[face][1], offset);
    Vector3 c = Vector3Add(face_vertices[face][2], offset);
    Vector3 d = Vector3Add(face_vertices[face][3], offset);

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

const char *get_camera_direction_string(Camera *camera)
{
    Vector3 camera_target = Vector3Subtract(camera->target, camera->position);
    camera_target.y = 0;
    camera_target = Vector3Normalize(camera_target);

    f32 look_at_angle_x = Vector3DotProduct((Vector3){1.0f, 0.0f, 0.0f}, camera_target);
    f32 look_at_angle_z = Vector3DotProduct((Vector3){0.0f, 0.0f, 1.0f}, camera_target);

    if (look_at_angle_x > 0.5f)       return "East (positive X)";
    else if (look_at_angle_x < -0.5f) return "West (negative X)";
    else if (look_at_angle_z > 0.5f)  return "South (positive Z)";
    else if (look_at_angle_z < -0.5f) return "North (negative Z)";

    return "";
}

static inline void chunk_set_block(Chunk *c, int x, int y, int z, Block_Kind type)
{
    c->blocks[CHUNK_IDX(x, y, z)] = type;
    c->dirty = true;
}

static inline Block_Kind chunk_get_block(Chunk *c, i32 x, i32 y, i32 z)
{
    return c->blocks[CHUNK_IDX(x, y, z)];
}

static bool neighbor_is_transparent(Chunk *c, i32 x, i32 y, i32 z)
{
    if (x < 0 || x >= CHUNK_SIZE ||
        y < 0 || y >= CHUNK_SIZE ||
        z < 0 || z >= CHUNK_SIZE)
    {
        return true;
    }

    return block_is_transparent(chunk_get_block(c, x, y, z));
}

static bool chunk_face_visible(Chunk *c, i32 x, i32 y, i32 z, Block_Face face)
{
    Vector3 dir = face_normals[face];
    i32 nx = x + (i32)dir.x;
    i32 ny = y + (i32)dir.y;
    i32 nz = z + (i32)dir.z;
    return neighbor_is_transparent(c, nx, ny, nz);
}

u32 count_visible_faces(Chunk *c, i32 x, i32 y, i32 z)
{
    u32 faces = 0;
    if (neighbor_is_transparent(c, x + 1, y, z)) faces++;
    if (neighbor_is_transparent(c, x - 1, y, z)) faces++;
    if (neighbor_is_transparent(c, x, y + 1, z)) faces++;
    if (neighbor_is_transparent(c, x, y - 1, z)) faces++;
    if (neighbor_is_transparent(c, x, y, z + 1)) faces++;
    if (neighbor_is_transparent(c, x, y, z - 1)) faces++;
    return faces;
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
                faces += count_visible_faces(c, cx, cy, cz);
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
                if (block_is_transparent(kind)) continue;
                for (Block_Face face = 0; face < __count_block_face; ++face) {
                    if (chunk_face_visible(c, cx, cy, cz, face))
                        v = add_face(&c->mesh, face, cx, cy, cz, v);
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

void chunk_free(Chunk *c)
{
    UnloadMesh(c->mesh);

}

i32 main(void)
{
    InitWindow(WIDTH, HEIGHT, "Lithos");
    SetWindowMonitor(0); // force primary monitor
    SetTargetFPS(240);

    DisableCursor();

    Camera camera  = {
        .position = (Vector3){-2.0f, 1.625f, 0.0f},
        .target = (Vector3){0.0f, 1.625f, 0.0f},
        .up = (Vector3){0.0f, 1.0f, 0.0f},
        .fovy = 85.0f,
        .projection = CAMERA_PERSPECTIVE,
    };

    Chunk chunk = {0};
    chunk_set_block(&chunk, 0, 0, 0, BLOCK_STONE);
    chunk_set_block(&chunk, 1, 0, 0, BLOCK_STONE);
    chunk_set_block(&chunk, 1, 0, 1, BLOCK_STONE);
    chunk_set_block(&chunk, 0, 0, 1, BLOCK_STONE);

    if (chunk.dirty) {
        chunk_generate_mesh(&chunk);
    }

    while (!WindowShouldClose()) {
        // Update
        UpdateCamera(&camera, CAMERA_FREE);
        
        // Draw
        BeginDrawing();
        {
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
            {
                DrawGrid(100, 1.0f);
                DrawLine3D(Vector3Zero(), (Vector3){1.0f, 0.0f, 0.0f}, RED);
                DrawLine3D(Vector3Zero(), (Vector3){0.0f, 1.0f, 0.0f}, GREEN);
                DrawLine3D(Vector3Zero(), (Vector3){0.0f, 0.0f, 1.0f}, BLUE);

                DrawModel(chunk.model, Vector3Zero(), 1.0f, WHITE);
            }
            EndMode3D();
            DrawText(TextFormat("Lithos Alpha - FPS: %d", GetFPS()), 0, 0, 20, BLACK);
            DrawText(get_camera_direction_string(&camera), 0, 20, 20, BLACK);
        } 
        EndDrawing();
    }

    chunk_free(&chunk);

    return 0;
}
