#include <math.h>
#include "ships.h"
#include "config.h"

// Distance between two points
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Maximum range at 45 degrees (angle that gives max range)
double maxRange(double v) {
    double thetaRad = 45.0 * M_PI / 180.0;
    return (v * v * sin(2 * thetaRad)) / GRAVITY;
}

// Checks if target is within attack range
int isInRange(double shooterX, double shooterY, double targetX, double targetY, double v) {
    double d = distance(shooterX, shooterY, targetX, targetY);
    double range = maxRange(v);
    return (d <= range);
}

// Time of flight, assuming 45 degree firing angle
double timeToHit(double v) {
    double thetaRad = 45.0 * M_PI / 180.0;
    return (2 * v * sin(thetaRad)) / GRAVITY;
}
