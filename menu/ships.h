#ifndef SHIPS_H
#define SHIPS_H

typedef struct {
    int id;
    char type;
    double x, y;
    double vMax, vMin;
    double angleMin, angleMax;
    double impactPower;
    double firingInterval;   // TE - firing interval, varies by escort ship type
    int destroyed;
} EscortShip;

typedef struct {
    char type;
    double x, y;
    double vMax;
    int destroyed;
} Battleship;

#endif
