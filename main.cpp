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
#include "spatialHasher.h"

spatialGrid screenGrid; // filled in inside main, see below
std::vector<Particle> particle_list {};

int main() { /*responsible for creating particle_list and bringing all functions together*/

    // must be done here and not as a global, otherwise it can run before config.cpp has set ballDiameter (it would be 0)
    screenGrid = initializeSpatialGrid();

    InitWindow(worldWidth * worldToScreenScale, worldHeight * worldToScreenScale, "Particle Engine");
    SetTargetFPS(framerate);

    /*actual loop that runs once per frame*/
    while (!WindowShouldClose()){
        for (int substepCounter=0; substepCounter < numberOfSubsteps; substepCounter++){
            verletIntegrationMover(particle_list, dt/numberOfSubsteps);
            clearSpatialGrid(screenGrid);
            reconstructSpatialGrid(screenGrid, particle_list);
            compute9ClosestCellsBalls(particle_list, screenGrid);
            particleWallCollisions(particle_list);
        }
        particleSpawnOnClick(particle_list);
        drawScreen(particle_list);
    }
    CloseWindow();
}
