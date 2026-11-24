#pragma once
#include <vector>
#include "particle.h"

struct spatialGrid {int totalRows; int totalColumns; double cellWidth; std::vector<int> cumulativeCellList;};


spatialGrid initializeSpatialGrid();
void reconstructSpatialGrid (spatialGrid&screenGrid, std::vector<Particle>&particle_list); // also sorts particle_list by cell
void compute9ClosestCellsBalls(std::vector<Particle>&particle_list, spatialGrid&screenGrid);

//check if i have put includes in the right place or if they are unnecessary