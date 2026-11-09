#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"

struct Vec2 {double x; double y;};
struct Particle {Vec2 position; Vec2 Velocity;};

Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}
double framerate = 60;
double worldHeight = 60.0; // Assuming a window height of 600 pixels
double worldWidth = 80.0; // Assuming a window width of 800 pixels


double WorldToScreenX(double x) {return {x * 10};}
double WorldToScreenY(double y) {return {worldHeight*10 - (y * 10)};}

int main() {
    Particle particle1 {{0.0,0.0},{10.0,30.0}};
    Particle particle2 {{5.0,5.0},{20.0,30.0}};
    Particle particle3 {{0.0,30.0},{15.0,10.0}};
    Particle particle4 {{30.0,0.0},{4.0,32.0}};

    Vec2 gravity{0.0, -9.81};
    double dt = 1.0/framerate;

    std::vector<Particle> particle_list {particle1, particle2, particle3, particle4};
    particle_list.reserve(10000);

    for (Particle particle : particle_list){
        std::cout << "Particle position: (" << particle.position.x << ", " << particle.position.y << ")\n";
    }

    InitWindow(worldWidth * 10, worldHeight * 10, "Particle Engine");
    SetTargetFPS(framerate);

    while (!WindowShouldClose()){
        ClearBackground(BLACK);
        DrawText(("FPS:" + std::to_string(1/GetFrameTime())).c_str(), 10, 10, 20, WHITE);
        DrawText(("Balls:" + std::to_string(particle_list.size())).c_str(), 10, 30, 20, WHITE);


        for (Particle &particle : particle_list){
            particle.Velocity = particle.Velocity + gravity * dt;
            particle.position = particle.position + particle.Velocity * dt;
            if (particle.position.y < 0 or particle.position.y > worldHeight){
                particle.Velocity.y *= -1;
                particle.position.y = (particle.position.y < 0) ? 0 : worldHeight;
            }
            if (particle.position.x < 0 or particle.position.x > worldWidth){
                particle.Velocity.x *= -1;
                particle.position.x = (particle.position.x < 0) ? 0 : worldWidth;
            }
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Particle new_particle {{(double)GetMouseX()/10, (double)(worldHeight*10 - GetMouseY())/10}, {GetRandomValue(-20, 20), GetRandomValue(-20, 20)}};
            particle_list.push_back(new_particle);
        }

        BeginDrawing();
        for (Particle particle : particle_list){
            DrawCircle(WorldToScreenX(particle.position.x), WorldToScreenY(particle.position.y), 5, RED);
        }
        EndDrawing();
    }

    CloseWindow();
}