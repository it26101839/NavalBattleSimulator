#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ships.h"
#include "setup.h"
#include "physics.h"
#include "fileio.h"

int main() {
    srand(time(NULL));

    double D = 1000.0;
    int N = 5;
    double vMaxB = 800.0;

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    printf("Battleship at (%.2f, %.2f), Vmax = %.2f\n\n", b.x, b.y, b.vMax);
    printEscortShips(escorts, N);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    printf("\n--- Part 1-A: Attack Check ---\n");

    int hitCount = 0;
    int bSunkBy = -1;
    double totalBattleTime = 0.0;

    int hitIds[100];
    double hitTimes[100];

    for (int i = 0; i < N && bSunkBy == -1; i++) {
        if (isInRange(escorts[i].x, escorts[i].y, b.x, b.y, escorts[i].vMax)) {
            bSunkBy = escorts[i].id;
            b.destroyed = 1;
            break;
        }
        if (isInRange(b.x, b.y, escorts[i].x, escorts[i].y, b.vMax)) {
            escorts[i].destroyed = 1;

            double t = timeToHit(b.vMax);
            hitIds[hitCount] = escorts[i].id;
            hitTimes[hitCount] = t;

            if (t > totalBattleTime) {
                totalBattleTime = t;
            }

            hitCount++;
        }
    }

    if (b.destroyed) {
        printf("Battleship SUNK by E%d\n", bSunkBy);
    } else {
        printf("Battleship survives.\n");
        printf("Total E ships hit by B: %d\n", hitCount);
        printf("Time taken to end battle: %.4f seconds\n", totalBattleTime);
        for (int i = 0; i < hitCount; i++) {
            printf("  E%d hit at t = %.4f s\n", hitIds[i], hitTimes[i]);
        }
    }

    saveFinalResults("final_results.txt", &b, escorts, N, bSunkBy, hitCount, hitIds, hitTimes, totalBattleTime);

    return 0;
}
