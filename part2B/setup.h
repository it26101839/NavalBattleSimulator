#ifndef SETUP_H
#define SETUP_H
#include "ships.h"

void initEscortShips(EscortShip ships[], int n, double D, double vMaxB);
void printEscortShips(EscortShip ships[], int n);
void initBattleship(Battleship *b, char type, double x, double y, double vMax);

#endif
