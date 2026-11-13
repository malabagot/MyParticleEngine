#include "config.h"
#include <vector>
#include "particle.h"
#include <cmath>
#include "spatialHasher.h"
#include "physics.h"


spatialGrid initializeSpatialGrid(){
    std::vector<std::vector<int>>screenGridList{}; // instantiating the nested list for spatial grid
    spatialGrid screenGrid = {static_cast<int>(std::floor(worldWidth/ballDiameter)),static_cast<int>(std::floor(worldHeight/ballDiameter)), ballDiameter, screenGridList };
    //^ above is instantiating the struct for our spatial grid: totalrows, totalcolumns, cellWidth and the actual spatial grid list
    screenGrid.spatialGridList.resize(screenGrid.totalRows * screenGrid.totalColumns);
    return screenGrid;
};



void clearSpatialGrid (spatialGrid&screenGrid){
    for (int i = 0; i < screenGrid.spatialGridList.size(); i++){
        screenGrid.spatialGridList[i].clear();
    };
};


void reconstructSpatialGrid (spatialGrid&screenGrid, std::vector<Particle>&particle_list){
    /*for (Particle particle: particle_list){
        int row = std::floor(particle.currentPosition.x/ballDiameter);
        int column = std::floor(particle.currentPosition.y/ballDiameter);
        int index = row*screenGrid.totalColumns + column;
        screenGrid.spatialGridList[index].push_back(particle); //figure out whats going on here
    };*/

    for (int i=0; i < particle_list.size(); i++){
        int column  = std::floor(particle_list[i].currentPosition.x/ballDiameter);
        int row = std::floor(particle_list[i].currentPosition.y/ballDiameter);
        // keep the cell inside the grid so particles on the edge don't crash
        if (row < 0) row = 0;
        if (row >= screenGrid.totalRows) row = screenGrid.totalRows - 1;
        if (column < 0) column = 0;
        if (column >= screenGrid.totalColumns) column = screenGrid.totalColumns - 1;

        particle_list[i].currentRow = row; // store the cell on the particle so the 9 cell check can use it
        particle_list[i].currentColumn = column;
        int index = row*screenGrid.totalColumns + column;
        screenGrid.spatialGridList[index].push_back(i);
    };
};

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

