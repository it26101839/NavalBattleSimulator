#include <stdio.h>
#include <stdlib.h>
#include "ships.h"
#include "config.h"

void initEscortShips(EscortShip ships[], int n, double D, double vMaxB) {
    char typeChars[5] = {'A', 'B', 'C', 'D', 'E'};

    for (int i = 0; i < n; i++) {
        ships[i].id = i + 1;

        int typeIndex = rand() % 5;
        ships[i].type = typeChars[typeIndex];

        ships[i].x = ((double)rand() / RAND_MAX) * D;
        ships[i].y = ((double)rand() / RAND_MAX) * D;

        ships[i].impactPower = IMPACT_POWER[typeIndex];

        ships[i].angleMin = ((double)rand() / RAND_MAX) * 30;
        ships[i].angleMax = ships[i].angleMin + ANGLE_RANGE[typeIndex];

        if (typeIndex == 0) {
            ships[i].vMax = 1.2 * vMaxB;
        } else {
            ships[i].vMax = ((double)rand() / RAND_MAX) * vMaxB;
        }
        ships[i].vMin = ((double)rand() / RAND_MAX) * ships[i].vMax;

        ships[i].destroyed = 0;
    }
}

void printEscortShips(EscortShip ships[], int n) {
    for (int i = 0; i < n; i++) {
        printf("E%d | Type: %c | Pos: (%.2f, %.2f) | Vmax: %.2f | Impact: %.2f\n",
               ships[i].id, ships[i].type, ships[i].x, ships[i].y,
               ships[i].vMax, ships[i].impactPower);
    }
}

void initBattleship(Battleship *b, char type, double x, double y, double vMax) {
    b->type = type;
    b->x = x;
    b->y = y;
    b->vMax = vMax;
    b->destroyed = 0;
}
