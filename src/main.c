#include <assert.h>
#include <stdbool.h>
#include "core.h"

#include "raylib.h"
#include "raymath.h"

#include "chunk.h"

#define Vector3_Fmt "(X: %f Y: %f Z: %f)"
#define Vector3_Arg(vec) (vec).x, (vec).y, (vec).z

#define SCREEN_FACTOR 100
#define WIDTH 16 * SCREEN_FACTOR
#define HEIGHT 9 * SCREEN_FACTOR

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
