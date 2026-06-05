#include "core.h"

#include "raylib.h"
#include "raymath.h"

#define Vector3_Fmt "(X: %f Y: %f Z: %f)"
#define Vector3_Arg(vec) (vec).x, (vec).y, (vec).z

#define SCREEN_FACTOR 100
#define WIDTH 16 * SCREEN_FACTOR
#define HEIGHT 9 * SCREEN_FACTOR

#define CHUNK_SIZE 16

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
    BLOCK_STONE
} Block_Type;

typedef struct {
    u8 blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
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

i32 add_face(Mesh *mesh, Block_Face f, i32 v)
{
    Vector3 n = face_normals[f];

    Vector3 a = face_vertices[f][0];
    Vector3 b = face_vertices[f][1];
    Vector3 c = face_vertices[f][2];
    Vector3 d = face_vertices[f][3];

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

static Mesh gen_mesh_cube(void)
{
    Mesh mesh = {0};

    i32 faces = 6;
    i32 verts_per_face = 6;
    i32 total_verts = faces * verts_per_face;

    mesh.vertexCount = total_verts;
    mesh.triangleCount = faces * 2;

    mesh.vertices = (f32 *)MemAlloc(total_verts * 3 * sizeof(f32));
    mesh.normals = (f32 *)MemAlloc(total_verts * 3 * sizeof(f32));
    mesh.texcoords = (f32 *)MemAlloc(total_verts * 2 * sizeof(f32));

    i32 v = 0;
    for (u32 i = 0; i < __count_block_face; ++i) {
        v = add_face(&mesh, i, v);
    }

    UploadMesh(&mesh, false);
    return mesh;
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

    Image checked = GenImageChecked(2, 2, 1, 1, RED, GREEN);
    Texture2D texture = LoadTextureFromImage(checked);
    UnloadImage(checked);

    Model cube_model = LoadModelFromMesh(gen_mesh_cube());
    cube_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

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

                DrawModel(cube_model, Vector3Zero(), 1.0f, WHITE);
            }
            EndMode3D();
            DrawText(TextFormat("Lithos Alpha - FPS: %d", GetFPS()), 0, 0, 20, BLACK);
            DrawText(get_camera_direction_string(&camera), 0, 20, 20, BLACK);
        } 
        EndDrawing();
    }

    return 0;
}
