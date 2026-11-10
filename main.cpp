#include "config.h"
#include "vec2.h"
#include "particle.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "raylib.h"
#include "render.h"
#include "physics.h"
#include "playerInteraction.h"

int main() { /*responsible for creating particle_list and bringing all functions together*/
    /*we create 4 dummy particles to start*/
    Particle particle1 {{31.00, 31.00}, {31.05, 31.15}, {0, -9.81}};
    Particle particle2 {{30.50, 31.50}, {30.60, 31.70}, {0, -9.81}};
    Particle particle3 {{31.50, 30.50}, {31.45, 30.70}, {0, -9.81}};
    Particle particle4 {{30.20, 30.20}, {30.30, 30.45}, {0, -9.81}};

    std::vector<Particle> particle_list {particle1, particle2, particle3, particle4};
    particle_list.reserve(10000);

    InitWindow(worldWidth * worldToScreenScale, worldHeight * worldToScreenScale, "Particle Engine");
    SetTargetFPS(framerate);

    /*actual loop that runs once per frame*/
    while (!WindowShouldClose()){
        for (int substepCounter=0; substepCounter < numberOfSubsteps; substepCounter++){
            verletIntegrationMover(particle_list, dt/numberOfSubsteps);
            particleBallCollisions(particle_list);
            particleWallCollisions(particle_list);
        }
        particleSpawnOnClick(particle_list);
        drawScreen(particle_list);
    }
    CloseWindow();
}
