#pragma once
#include <vector>
#include "particle.h"

void particleBallCollisions(std::vector<Particle>& particles);
void particleWallCollisions(std::vector<Particle>& particles);
void verletIntegrationMover(std::vector<Particle>& particle_list, double dt);