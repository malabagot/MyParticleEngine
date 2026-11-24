#include "config.h"
#include <vector>
#include "particle.h"
#include <cmath>
#include "spatialHasher.h"
#include "physics.h"


spatialGrid initializeSpatialGrid(){ //must return the 2
    int totalRows = ceil(worldHeight/ballDiameter);
    int totalColumns = ceil(worldWidth/ballDiameter);
    double cellWidth = ballDiameter;

    std::vector<int> cumulativeCellList {};
     //this list shoes the following:
    // each index is a cell, and the value is where that cell's particles START in particle_list (which is sorted by cell)
    // e.g. particle counts per cell [2,0,1,3] -> cumulativeCellList [0,2,2,3,6]
    // cell c's particles are from cumulativeCellList[c] up to (not including) cumulativeCellList[c+1]
    // there is one extra entry at the end (the total number of particles) so the last cell also has an end

    return {totalRows, totalColumns, cellWidth, cumulativeCellList};
};


void reconstructSpatialGrid (spatialGrid&screenGrid, std::vector<Particle>&particle_list){
    //this for loop calculates the row and column of each particle, and counts how many particles are in each cell
    std::vector<int> tempList {};
    tempList.resize(screenGrid.totalColumns*screenGrid.totalRows); //temporary non-cumulative list to count number of balls in each cell.

    for (int i=0; i < particle_list.size(); i++){ //for every particle
        int column  = std::floor(particle_list[i].currentPosition.x/ballDiameter); //calculate the cell row and column they are in
        int row = std::floor(particle_list[i].currentPosition.y/ballDiameter);
        
        // keep the cell inside the grid so particles on the edge don't crash
        if (row < 0) row = 0;
        if (row >= screenGrid.totalRows) row = screenGrid.totalRows - 1;
        if (column < 0) column = 0;
        if (column >= screenGrid.totalColumns) column = screenGrid.totalColumns - 1;
        int cell = row*screenGrid.totalColumns + column;

        particle_list[i].currentRow = row; // store the cell on the particle so the 9 cell check can use it
        particle_list[i].currentColumn = column;
        
        tempList[cell] ++; // add to list to say how many balls are in that cell
        };

        //turning tempList into the cumulative list
        screenGrid.cumulativeCellList.resize(tempList.size() + 1); // +1 for the extra end entry
        int counter = 0;
        for (int i=0;i < tempList.size();i++){ //for every cell in the temp list
            screenGrid.cumulativeCellList[i] = counter; // where this cell starts
            counter = counter + tempList[i];
            };

        screenGrid.cumulativeCellList[tempList.size()] = counter; // end of the last cell (total particles)



        // we just finished recreating the cumulative list, now its time to put the particles into cell order.
        static std::vector<Particle> temporary_particle_list {}; // static so it isnt recreated every call
        temporary_particle_list.resize(particle_list.size());

        std::vector<int> nextFreeSlot {};
        nextFreeSlot.resize(tempList.size()); // as we fill the sorted list, we must be able to see how many of the particles have been added to the cell already

        for (int i=0; i < particle_list.size();i++){ //for all particles
            int cell = particle_list[i].currentRow*screenGrid.totalColumns + particle_list[i].currentColumn; //calculate that particles cell index
            temporary_particle_list[screenGrid.cumulativeCellList[cell] + nextFreeSlot[cell]] = particle_list[i]; // put the particle in the next free slot of its cell
            nextFreeSlot[cell] ++;
        };
        particle_list.swap(temporary_particle_list); // particle_list is now sorted by cell
    };

/* OLD VERSION (used spatialGridList)
void compute9ClosestCellsBalls(std::vector<Particle>&particle_list, spatialGrid&screenGrid){ //checks for all balls if there are balls in the closest 9 grid cells and calls particleBallCollisions if there is
    for (int mainParticleIndex = 0; mainParticleIndex < particle_list.size(); mainParticleIndex++){ //for the list of all particles
        Particle&particle = particle_list[mainParticleIndex]; //let particle be the particle found from the main index we are looking at
        for(int rowOffset=-1; rowOffset < 2; rowOffset++){ // this combined with next line checks all 9 closest spaces in grid
            for(int columnOffset=-1; columnOffset < 2; columnOffset++){
                int neighbourRow = particle.currentRow + rowOffset;
                int neighbourColumn = particle.currentColumn + columnOffset;
                if(neighbourRow >= 0 && neighbourRow < screenGrid.totalRows && neighbourColumn >= 0 && neighbourColumn < screenGrid.totalColumns){
                    for(int secondParticleIndex : screenGrid.spatialGridList[neighbourRow*screenGrid.totalColumns + neighbourColumn]){ //for all particles indexes in that grid box
                        if(mainParticleIndex != secondParticleIndex){ //checks we arent comparing the same ball we are looking at
                            particleBallCollisions(particle_list,mainParticleIndex,secondParticleIndex);
                        };
                    };
                }
            };
        };
    };
};
*/

void compute9ClosestCellsBalls(std::vector<Particle>&particle_list, spatialGrid&screenGrid){ //checks for all balls if there are balls in the closest 9 grid cells and calls particleBallCollisions if there is
    for (int mainParticleIndex = 0; mainParticleIndex < particle_list.size(); mainParticleIndex++){ //for every particle
        Particle&particle = particle_list[mainParticleIndex];

        for(int rowOffset=-1; rowOffset < 2; rowOffset++){ // this combined with next line checks all 9 closest cells
            for(int columnOffset=-1; columnOffset < 2; columnOffset++){
                int neighbourRow = particle.currentRow + rowOffset;
                int neighbourColumn = particle.currentColumn + columnOffset;

                if(neighbourRow >= 0 && neighbourRow < screenGrid.totalRows && neighbourColumn >= 0 && neighbourColumn < screenGrid.totalColumns){ //skip cells outside the grid
                    int cell = neighbourRow*screenGrid.totalColumns + neighbourColumn;
                    int cellStart = screenGrid.cumulativeCellList[cell];   // where this cell's particles start in particle_list
                    int cellEnd = screenGrid.cumulativeCellList[cell + 1]; // where the next cell starts (so this cell ends)

                    for(int secondParticleIndex = cellStart; secondParticleIndex < cellEnd; secondParticleIndex++){ //for every particle in this cell (particle_list is sorted, so the index is the position)
                        if(mainParticleIndex != secondParticleIndex){ //checks we arent comparing the ball with itself
                            particleBallCollisions(particle_list, mainParticleIndex, secondParticleIndex);
                        };
                    };
                };
            };
        };
    };
};

