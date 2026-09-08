#ifndef SHIPS_H
#define SHIPS_H

typedef struct {
    int id;
    char type;
    double x, y;
    double vMax, vMin;
    double angleMin, angleMax;
    double impactPower;
    int destroyed;
} EscortShip;

typedef struct {
    char type;
    double x, y;
    double vMax;
    int destroyed;
} Battleship;

#endif
