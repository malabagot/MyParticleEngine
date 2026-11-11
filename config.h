#pragma once


extern double framerate;
extern double worldHeight; // window height of 600 pixels
extern double worldWidth ; // window width of 800 pixels
extern double ballRadius ;
extern double ballDiameter;
extern double worldToScreenScale; 
extern double dt;
extern double dampingCoefficient;
extern int numberOfSubsteps;
double WorldToScreenX(double x);
double WorldToScreenY(double y);
