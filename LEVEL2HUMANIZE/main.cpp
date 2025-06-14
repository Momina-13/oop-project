
#include "raylib.h"
#include "game.h"

int main() {

    InitWindow(800, 700, "Battle Arena");


    SetTargetFPS(60);

    BattleArenaGame game;

    while (!WindowShouldClose()) {
        game.Update(); 

        BeginDrawing();
        ClearBackground(RAYWHITE);
        game.Draw();   
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
