#include "raylib.h"
#include "game_state.h"
#include "types.h"

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "BEEHIVE REVOLT");
    SetTargetFPS(TARGET_FPS);

    Game g = {0};
    assets_load(&g.assets);
    game_init(&g);

    while (!WindowShouldClose()) {
        game_frame(&g, GetFrameTime());
    }

    assets_unload(&g.assets);
    CloseWindow();
    return 0;
}
