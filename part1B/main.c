#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ships.h"
#include "setup.h"
#include "physics.h"
#include "fileio.h"
#include "simulate.h"

int main() {
    srand(time(NULL));

    double D = 100.0;
    int N = 5;
    double vMaxB = 800.0;
    int K = 5;   // katikama iterations gananaya

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    FILE *log = fopen("simulation1_log.txt", "w");
    fprintf(log, "=== Part 1-B Simulation 1: Battleship Moving Path ===\n\n");

    int hitIds[100];
    double hitTimes[100];
    double totalTime;
    int bSunkBy;
    int aliveCount = N;

    for (int iter = 1; iter <= K; iter++) {
        // B ekata random point ekak generate karanawa (path eke ekak)
        b.x = ((double)rand() / RAND_MAX) * D;
        b.y = ((double)rand() / RAND_MAX) * D;

        printf("\n--- Iteration %d: B at (%.2f, %.2f) ---\n", iter, b.x, b.y);
        fprintf(log, "--- Iteration %d: B at (%.2f, %.2f) ---\n", iter, b.x, b.y);

        int hitCount = simulateAttack(&b, escorts, N, hitIds, hitTimes, &totalTime, &bSunkBy);

        if (b.destroyed) {
            printf("Battleship SUNK by E%d at iteration %d\n", bSunkBy, iter);
            fprintf(log, "Battleship SUNK by E%d\n\n", bSunkBy);
            break;
        } else {
            printf("B survives. E ships hit this round: %d\n", hitCount);
            fprintf(log, "B survives. E ships hit: %d, time: %.4f s\n", hitCount, totalTime);
            for (int i = 0; i < hitCount; i++) {
                fprintf(log, "  E%d hit at t=%.4f s\n", hitIds[i], hitTimes[i]);
                aliveCount--;
            }
            fprintf(log, "\n");
        }

        if (aliveCount <= 0) {
            printf("All escort ships destroyed!\n");
            fprintf(log, "All escort ships destroyed!\n");
            break;
        }
    }

    fclose(log);
    saveFinalResults("final_results.txt", &b, escorts, N, bSunkBy, 0, hitIds, hitTimes, totalTime);

    return 0;
}
