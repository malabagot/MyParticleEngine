// Headless benchmark: runs the physics with no window and no rendering,
// then prints how long each frame took. Links against `physics` only.
#include <chrono>
#include <iostream>
#include <vector>
#include "config.h"
#include "particle.h"
#include "physics.h"
#include "spatialHasher.h"

// Benchmark settings
const int totalFrames = 600;    // 10 seconds of simulation at 60 fps
const int numberOfSamples = 100; // how many frames get timed during those 10 seconds
const int framesBetweenSamples = totalFrames / numberOfSamples; // = 6, so a frame is timed every 0.1 seconds
const double maxMsPerFrame = 1000; // stop testing if one frame takes over 1 second (brute force gets this slow)
const std::vector<int> particleCounts = {1000, 2500, 5000, 10000, 25000, 50000, 75000, 100000};

// Places `count` particles in neat rows starting at the bottom-left of the world,
// at rest, with the same gravity as particleSpawnOnClick uses.
std::vector<Particle> spawnParticles(int count) {
    std::vector<Particle> particle_list;
    double spacing = ballDiameter * 1.1; // small gap so they don't start overlapping
    int particlesPerRow = (int)((worldWidth - ballDiameter) / spacing);

    for (int i = 0; i < count; i++) {
        double x = ballRadius + (i % particlesPerRow) * spacing;
        double y = ballRadius + (i / particlesPerRow) * spacing;
        Particle p {{x, y}, {x, y}, {0.0, -30}, 0, 0}; // previous == current means velocity starts at 0
        particle_list.push_back(p);
    }
    return particle_list;
}

// One frame of physics: same substep loop as main.cpp, minus input and drawing.
void stepOneFrame(std::vector<Particle>& particle_list, spatialGrid& screenGrid) {
    for (int substepCounter = 0; substepCounter < numberOfSubsteps; substepCounter++) {
        verletIntegrationMover(particle_list, dt / numberOfSubsteps);
        if (useSpatialHashing) {
            reconstructSpatialGrid(screenGrid, particle_list);
            compute9ClosestCellsBalls(particle_list, screenGrid);
        } else {
            bruteForceBallCollisions(particle_list);
        }
        particleWallCollisions(particle_list);
    }
}

// Runs a 10 second simulation, timing one frame every `framesBetweenSamples` frames,
// and returns the average ms of those sampled frames.
// Returns -1 if any single frame takes longer than maxMsPerFrame (too slow to be worth finishing).
double runBenchmark(int particleCount) {
    std::vector<Particle> particle_list = spawnParticles(particleCount);
    spatialGrid screenGrid = initializeSpatialGrid();
    double totalSampledMs = 0;

    for (int frame = 0; frame < totalFrames; frame++) {
        // every frame is timed so we can spot a too-slow one, but only sample frames count towards the average
        auto start = std::chrono::steady_clock::now();
        stepOneFrame(particle_list, screenGrid);
        auto end = std::chrono::steady_clock::now();
        double frameMs = std::chrono::duration<double, std::milli>(end - start).count();

        if (frameMs > maxMsPerFrame) {
            return -1;
        }
        if (frame % framesBetweenSamples == 0) {
            totalSampledMs += frameMs;
        }
    }
    return totalSampledMs / numberOfSamples;
}

int main() {
    std::cout << "Mode: " << (useSpatialHashing ? "spatial hashing" : "brute force") << "\n";
    std::cout << totalFrames << " frames per run, " << numberOfSubsteps << " substeps per frame, "
              << numberOfSamples << " frames sampled\n\n";

    for (int count : particleCounts) {
        double averageMsPerFrame = runBenchmark(count);
        if (averageMsPerFrame < 0) {
            std::cout << count << " particles: a frame took over " << maxMsPerFrame / 1000 << " seconds, stopping here.\n";
            break; // bigger counts would only be slower
        }
        std::cout << count << " particles: "
                  << averageMsPerFrame << " ms/frame  (~"
                  << (int)(1000.0 / averageMsPerFrame) << " fps physics only)\n";
    }
}
