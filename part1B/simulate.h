#ifndef SIMULATE_H
#define SIMULATE_H
#include "ships.h"

int simulateAttack(Battleship *b, EscortShip escorts[], int n,
                    int hitIds[], double hitTimes[], double *totalTime,
                    int *bSunkBy);

#endif
