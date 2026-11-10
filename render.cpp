#include <vector>
#include "render.h"
#include "raylib.h"
#include <iostream>
#include "config.h"
#include "particle.h"

void drawScreen(std::vector<Particle>&particle_list){
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText(("FPS:" + std::to_string(GetFPS())).c_str(), 10, 10, 20, WHITE);
    DrawText(("Balls:" + std::to_string(particle_list.size())).c_str(), 10, 30, 20, WHITE);
    for (Particle particle : particle_list){
        DrawCircle(WorldToScreenX(particle.currentPosition.x), WorldToScreenY(particle.currentPosition.y), ballRadius * worldToScreenScale, RED);
    }
    EndDrawing();
}