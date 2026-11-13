#include "config.h"
#include "vec2.h"
#include "particle.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "physics.h"
#include "spatialHasher.h"

spatialGrid screenGrid; // filled in inside main, see below
std::vector<Particle> particle_list {};

// benchmark settings
std::vector<int> ballCountsToTest {1000, 2500, 5000, 10000, 25000, 50000, 75000, 100000};
double secondsPerTest = 10.0;
int samplesPerTest = 100; // we record the ms of one frame 100 times spread evenly over the 10 seconds
double lowestAcceptableFPS = 5.0; // stop testing bigger ball counts once the average FPS drops below this

void spawnBalls(std::vector<Particle>& particle_list, int numberOfBalls) { // fills the list with balls at random positions, same positions every run because of the fixed seed
    particle_list.clear();
    std::srand(42);
    for (int i = 0; i < numberOfBalls; i++){
        double x = ballRadius + (std::rand() % 1000) / 1000.0 * (worldWidth - ballDiameter);
        double y = ballRadius + (std::rand() % 1000) / 1000.0 * (worldHeight - ballDiameter);
        Particle new_particle {{x, y}, {x, y}, {0.0, -9.81}, 0, 0};
        particle_list.push_back(new_particle);
    }
}

void runOneFrame() { // exactly the same physics as the normal simulation, just no drawing
    for (int substepCounter=0; substepCounter < numberOfSubsteps; substepCounter++){
        verletIntegrationMover(particle_list, dt/numberOfSubsteps);
        if (useSpatialHashing){
            clearSpatialGrid(screenGrid);
            reconstructSpatialGrid(screenGrid, particle_list);
            compute9ClosestCellsBalls(particle_list, screenGrid);
        } else {
            bruteForceBallCollisions(particle_list);
        }
        particleWallCollisions(particle_list);
    }
}

int main() { /*runs the simulation without a window and prints how long each frame takes for different ball counts*/

    // must be done here and not as a global, otherwise it can run before config.cpp has set ballDiameter (it would be 0)
    screenGrid = initializeSpatialGrid();

    std::cout << "Mode: " << (useSpatialHashing ? "spatial hashing" : "brute force") << ", substeps: " << numberOfSubsteps << "\n";
    std::cout << "balls, average ms per frame, average FPS\n";

    for (int numberOfBalls : ballCountsToTest){
        spawnBalls(particle_list, numberOfBalls);

        double secondsBetweenSamples = secondsPerTest / samplesPerTest;
        double totalSampledMilliseconds = 0.0;
        int samplesTaken = 0;

        auto testStart = std::chrono::steady_clock::now();
        auto nextSampleTime = testStart;

        while (std::chrono::duration<double>(std::chrono::steady_clock::now() - testStart).count() < secondsPerTest){
            auto frameStart = std::chrono::steady_clock::now();
            runOneFrame();
            auto frameEnd = std::chrono::steady_clock::now();

            // only record this frame if we have reached the next sample point, so we get ~100 samples spread over the 10 seconds
            if (frameEnd >= nextSampleTime && samplesTaken < samplesPerTest){
                totalSampledMilliseconds += std::chrono::duration<double, std::milli>(frameEnd - frameStart).count();
                samplesTaken++;
                nextSampleTime += std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(secondsBetweenSamples));
            }
        }

        double averageMilliseconds = totalSampledMilliseconds / samplesTaken;
        double averageFPS = 1000.0 / averageMilliseconds;
        std::cout << numberOfBalls << ", " << averageMilliseconds << ", " << averageFPS << "\n";

        if (averageFPS < lowestAcceptableFPS){
            std::cout << "FPS too low, stopping here.\n";
            break;
        }
    }
}
