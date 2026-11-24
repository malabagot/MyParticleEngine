#include "config.h"

double framerate = 60;
double worldHeight = 80.0; //window height of 800 pixels
double worldWidth = 80.0; //window width of 800 pixels
double ballRadius = 0.1;
double ballDiameter = ballRadius * 2;
double worldToScreenScale = 10.0;
double dt = 1.0/framerate;
double dampingCoefficient = 0.9999; 
int numberOfSubsteps = 2;
double WorldToScreenX(double x) {return {x * worldToScreenScale};};
double WorldToScreenY(double y) {return {worldHeight * worldToScreenScale - (y * worldToScreenScale)};}

bool useSpatialHashing = true; 
