#ifndef SIMULATE_H
#define SIMULATE_H
#include "ships.h"

int simulateAttackPercent(Battleship *b, EscortShip escorts[], int n,
                           int hitIds[], double hitTimes[], double *totalTime,
                           double *bDamage);

#endif
