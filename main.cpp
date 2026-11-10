#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "raylib.h"


struct Vec2 {double x; double y;};
struct Particle {Vec2 previousPosition; Vec2 currentPosition; Vec2 acceleration;};

Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}
Vec2 operator/(Vec2 a, double b) {return {a.x/b, a.y/b};}
void operator+=(Vec2 &a, Vec2 b) {a.x += b.x; a.y += b.y;}
void operator-=(Vec2 &a, Vec2 b) {a.x -= b.x; a.y -= b.y;}

double framerate = 60;
double worldHeight = 60.0; // Assuming a window height of 600 pixels
double worldWidth = 80.0; // Assuming a window width of 800 pixels
double ballRadius = 0.5;
double worldToScreenScale = 10.0; // Scale factor for converting world coordinates to screen coordinates

double WorldToScreenX(double x) {return {x * worldToScreenScale};}
double WorldToScreenY(double y) {return {worldHeight * worldToScreenScale - (y * worldToScreenScale)};}

int main() {
    Particle particle1 {{31.00, 31.00}, {31.05, 31.15}, {0, -9.81}};
    Particle particle2 {{30.50, 31.50}, {30.60, 31.70}, {0, -9.81}};
    Particle particle3 {{31.50, 30.50}, {31.45, 30.70}, {0, -9.81}};
    Particle particle4 {{30.20, 30.20}, {30.30, 30.45}, {0, -9.81}};

    double dt = 1.0/framerate;

    std::vector<Particle> particle_list {particle1, particle2, particle3, particle4};
    particle_list.reserve(10000);

    InitWindow(worldWidth * worldToScreenScale, worldHeight * worldToScreenScale, "Particle Engine");
    SetTargetFPS(framerate);
    /*actual loop that runs once per frame*/
    while (!WindowShouldClose()){
        /*calculates new previous and current positiosn for verlet integrations*/
        for (Particle &particle : particle_list){
            Vec2 temp = particle.currentPosition;
            particle.currentPosition = particle.currentPosition + (particle.currentPosition - particle.previousPosition) + particle.acceleration * dt * dt;
            particle.previousPosition = temp;

        }
        /*collision checker for between any 2 balls*/
        for (int i=0; i<particle_list.size()-1; i++){
            for (int j=i+1; j<particle_list.size(); j++){
                //instantiating variables to use for checking distance between 2 balls
                double deltaCurrentPositionX = (particle_list[i].currentPosition.x - particle_list[j].currentPosition.x);
                double deltaCurrentPositionY = (particle_list[i].currentPosition.y - particle_list[j].currentPosition.y);
                double distanceBetweenPositionsSquared = double (deltaCurrentPositionX*deltaCurrentPositionX + deltaCurrentPositionY*deltaCurrentPositionY);

                Vec2 deltaCurrentPositionVector = (particle_list[j].currentPosition - particle_list[i].currentPosition);
                
                //if distance between balls is smaller than their radius combined (so ) (we assume all balls are same radius)
                if(ballRadius*ballRadius*4 - distanceBetweenPositionsSquared > 0 and distanceBetweenPositionsSquared > 0){/*the and statement makes sure the balls arent directly ontop of each other, if they are, we wait untill next frame where hopefully they arent anymore*/
                    double overlap = ballRadius*2 - sqrt(distanceBetweenPositionsSquared); /*we only calculate overlap once we know they overlap because sqrt is long to compute and we dont wanna do it for everysingle ball even when they are yet to overlap*/
                    Vec2 instantUnitVector = deltaCurrentPositionVector/sqrt(distanceBetweenPositionsSquared);
                    //this pushes the current position of i backwards by the vector in the same direction as the collision and j is pushed forwards - why? because we do j - i for distance between 2 vectors so pushing j forwards and i backwards evens it out, but only by how much they overlapped
                    particle_list[i].currentPosition -= instantUnitVector * overlap/2; /*Overlap is divided by 2 because they both overlap by the same amount,*/
                    particle_list[j].currentPosition += instantUnitVector * overlap/2; /* If you didnt divide by two they would bounce super fast away from each other*/
                }
            }
        }
        /*collision checker for the walls (we do it after ball collision to ensure balls dont get pushed out of box when wall & ball collisions occur in the same frame)*/
        for (Particle &particle : particle_list){
            /*collision checker for the walls*/
            Vec2 gap = particle.currentPosition - particle.previousPosition;
            // Floor checker
            if (particle.currentPosition.y < ballRadius){
                particle.currentPosition.y = ballRadius;
                particle.previousPosition.y = particle.currentPosition.y + gap.y * 0.6;
            }
            // Ceiling checker
            if (particle.currentPosition.y > worldHeight - ballRadius){
                particle.currentPosition.y = worldHeight - ballRadius;
                particle.previousPosition.y = particle.currentPosition.y + gap.y * 0.6;
            }
            // Left wall checker
            if (particle.currentPosition.x < ballRadius){
                particle.currentPosition.x = ballRadius;
                particle.previousPosition.x = particle.currentPosition.x + gap.x * 0.6;
            }
            // Right wall checker
            if (particle.currentPosition.x > worldWidth - ballRadius){
                particle.currentPosition.x = worldWidth - ballRadius;
                particle.previousPosition.x = particle.currentPosition.x + gap.x * 0.6;
            }
        }
        /*particle created for every frame mouse button is held down*/
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            Particle new_particle {
                {
                    GetRandomValue(-100,100)/500.0+(double)GetMouseX()/worldToScreenScale,
                    GetRandomValue(-100,100)/500.0+(double)(worldHeight*worldToScreenScale - GetMouseY())/worldToScreenScale
                },
                {
                    (double)GetMouseX()/worldToScreenScale,
                    (double)(worldHeight*worldToScreenScale - GetMouseY())/worldToScreenScale
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
            DrawCircle(WorldToScreenX(particle.currentPosition.x), WorldToScreenY(particle.currentPosition.y), ballRadius * worldToScreenScale, RED);
        }
        EndDrawing();
    }

    CloseWindow();
}