#include "physics.h"
#include <cmath>
#include "config.h"
#include "particle.h"
#include "vec2.h" 

void verletIntegrationMover(std::vector<Particle>&particle_list, double dt) {
    for (Particle &particle : particle_list){
            Vec2 temp = particle.currentPosition;
            particle.currentPosition = particle.currentPosition + (particle.currentPosition - particle.previousPosition)*dampingCoefficient + particle.acceleration * dt * dt;
            particle.previousPosition = temp;

        }
}

void particleBallCollisions(std::vector<Particle>&particle_list, int i, int j) { // takes as input 2 balls and checks if they collide, if they do, calculates the new current position to simulate the bounce
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

void particleWallCollisions(std::vector<Particle>&particle_list) {
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
}

void bruteForceBallCollisions(std::vector<Particle>&particle_list) { // naive version: checks every pair of balls, only used when useSpatialHashing is false
    for (int i=0; i<particle_list.size()-1; i++){
        for (int j=i+1; j<particle_list.size(); j++){
            particleBallCollisions(particle_list, i, j);
        }
    }
}
