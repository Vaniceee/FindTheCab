#include "raylib.h"
#include "Player.h"

int main() {
    // Explicitly configure your window sizes here
    const int windowWidth = 1280;
    const int windowHeight = 720;

    InitWindow(windowWidth, windowHeight, "RPG Character Template - Screen Clamped");
    SetTargetFPS(60);

    // Spawns the player right in the dead center of the screen size bounds
    Player player({ (float)windowWidth / 2.0f, (float)windowHeight / 2.0f });

    while (!WindowShouldClose()) {

        // Pass window constraints straight to the update loop pipeline
        player.Update(windowWidth, windowHeight);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        player.Draw();

        DrawText("Use WASD/Arrows to Move. Hold LEFT SHIFT to Run.", 15, 15, 20, LIGHTGRAY);
        DrawText("Screen bounds clamped safely!", 15, windowHeight - 30, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}