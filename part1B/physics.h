#ifndef PHYSICS_H
#define PHYSICS_H

double distance(double x1, double y1, double x2, double y2);
double maxRange(double v);
int isInRange(double shooterX, double shooterY, double targetX, double targetY, double v);
double timeToHit(double v);

#endif
