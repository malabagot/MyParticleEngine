#include <iostream>
#include <string>
#include <vector>
#include "raylib.h"

struct Vec2 {double x; double y;};
struct Particle {Vec2 previousPosition; Vec2 currentPosition; Vec2 acceleration;};

Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}
double framerate = 60;
double worldHeight = 60.0; // Assuming a window height of 600 pixels
double worldWidth = 80.0; // Assuming a window width of 800 pixels
double ballRadius = 0.5;

double WorldToScreenX(double x) {return {x * 10};}
double WorldToScreenY(double y) {return {worldHeight*10 - (y * 10)};}

int main() {
    Particle particle1 {{31.00, 31.00}, {31.05, 31.15}, {0, -9.81}};
    Particle particle2 {{30.50, 31.50}, {30.60, 31.70}, {0, -9.81}};
    Particle particle3 {{31.50, 30.50}, {31.45, 30.70}, {0, -9.81}};
    Particle particle4 {{30.20, 30.20}, {30.30, 30.45}, {0, -9.81}};

    Vec2 gravity{0.0, -9.81};
    double dt = 1.0/framerate;

    std::vector<Particle> particle_list {particle1, particle2, particle3, particle4};
    particle_list.reserve(10000);

    InitWindow(worldWidth * 10, worldHeight * 10, "Particle Engine");
    SetTargetFPS(framerate);
    /*actual loop that runs once per frame*/
    while (!WindowShouldClose()){
        /*calculates new previous and current positiosn for verlet integration and does collision checks*/
        for (Particle &particle : particle_list){
            Vec2 temp = particle.currentPosition;
            particle.currentPosition = particle.currentPosition + (particle.currentPosition - particle.previousPosition) + particle.acceleration * dt * dt;
            particle.previousPosition = temp;
            Vec2 gap = particle.currentPosition - particle.previousPosition;
            // Floor and ceiling checker
            if (particle.currentPosition.y < ballRadius){
                particle.currentPosition.y = ballRadius;
                particle.previousPosition.y = particle.currentPosition.y + gap.y * 0.8;
            }
            if (particle.currentPosition.y > worldHeight - ballRadius){
                particle.currentPosition.y = worldHeight - ballRadius;
                particle.previousPosition.y = particle.currentPosition.y + gap.y * 0.8;
            }
            // Left and right walls chcker
            if (particle.currentPosition.x < ballRadius){
                particle.currentPosition.x = ballRadius;
                particle.previousPosition.x = particle.currentPosition.x + gap.x * 0.8;
            }
            if (particle.currentPosition.x > worldWidth - ballRadius){
                particle.currentPosition.x = worldWidth - ballRadius;
                particle.previousPosition.x = particle.currentPosition.x + gap.x * 0.8;
            }
        }
        /*collision checker for between any 2 balls*/
        /*for (int i=0; i<particle_list.size()+1; i++){
            for (int j=i+1; j<particle_list.size(); j++){
                //check for top and bottom (ternary checker)

                //check for left and right (also ternary checker)

            }
        }*/
        /*particle created for every frame mouse button is held down*/
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Particle new_particle {
                {
                    GetRandomValue(-100,100)/500.0+(double)GetMouseX()/10.0, 
                    GetRandomValue(-100,100)/500.0+(double)(worldHeight*10.0 - GetMouseY())/10.0
                },
                {
                    (double)GetMouseX()/10.0, 
                    (double)(worldHeight*10.0 - GetMouseY())/10.0
                },
                {0.0, -9.81
                }
            };
            particle_list.push_back(new_particle);
        }
        /*draws the actual circles, the text and the black background*/
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(("FPS:" + std::to_string(GetFPS())).c_str(), 10, 10, 20, WHITE);
        DrawText(("Balls:" + std::to_string(particle_list.size())).c_str(), 10, 30, 20, WHITE);
        for (Particle particle : particle_list){
            DrawCircle(WorldToScreenX(particle.currentPosition.x), WorldToScreenY(particle.currentPosition.y), ballRadius * 10, RED);
        }
        EndDrawing();
    }

    CloseWindow();
}