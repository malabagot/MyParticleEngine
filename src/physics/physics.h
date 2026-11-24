#pragma once
#include <vector>
#include "particle.h"

void particleBallCollisions(std::vector<Particle>&particle_list, int i, int j);
void particleWallCollisions(std::vector<Particle>& particles);
void verletIntegrationMover(std::vector<Particle>& particle_list, double dt);
void bruteForceBallCollisions(std::vector<Particle>&particle_list);