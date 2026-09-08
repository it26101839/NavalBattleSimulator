#ifndef FILEIO_H
#define FILEIO_H
#include "ships.h"

void saveInitialConditions(const char *filename, Battleship *b, EscortShip ships[], int n, double D);
void saveFinalResults(const char *filename, Battleship *b, EscortShip ships[], int n,
                       int bSunkBy, int hitCount, int hitIds[], double hitTimes[], double totalTime);

#endif
