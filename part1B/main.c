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

    double D = 1000.0;
    int N = 5;
    double vMaxB = 800.0;
    int K = 5;                // total number of iterations
    int T = 2;                // iteration after which the gun jams
    double thetaMin = 15.0;   // minimum angle allowed after jam

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    int hitIds[100];
    double hitTimes[100];
    double totalTime;
    int bSunkBy;

    // ============ SIMULATION 1 ============
    FILE *log1 = fopen("simulation1_log.txt", "w");
    fprintf(log1, "=== Part 1-B Simulation 1: Battleship Moving Path ===\n\n");

    EscortShip escorts1[N];
    for (int i = 0; i < N; i++) escorts1[i] = escorts[i];  // copy of the original state

    Battleship b1 = b;

    for (int iter = 1; iter <= K; iter++) {
        b1.x = ((double)rand() / RAND_MAX) * D;
        b1.y = ((double)rand() / RAND_MAX) * D;

        fprintf(log1, "--- Iteration %d: B at (%.2f, %.2f) ---\n", iter, b1.x, b1.y);

        int hitCount = simulateAttack(&b1, escorts1, N, hitIds, hitTimes, &totalTime, &bSunkBy);

        if (b1.destroyed) {
            fprintf(log1, "Battleship SUNK by E%d\n\n", bSunkBy);
            break;
        } else {
            fprintf(log1, "B survives. E ships hit: %d, time: %.4f s\n", hitCount, totalTime);
            for (int i = 0; i < hitCount; i++) {
                fprintf(log1, "  E%d hit at t=%.4f s\n", hitIds[i], hitTimes[i]);
            }
            fprintf(log1, "\n");
        }
    }
    fclose(log1);
    printf("Simulation 1 done. Check simulation1_log.txt\n");

    // ============ SIMULATION 2 (Gun Jam) ============
    FILE *log2 = fopen("simulation2_log.txt", "w");
    fprintf(log2, "=== Part 1-B Simulation 2: Gun Jam after iteration %d ===\n\n", T);

    EscortShip escorts2[N];
    for (int i = 0; i < N; i++) escorts2[i] = escorts[i];  // same initial conditions

    Battleship b2 = b;
    int jammed = 0;

    for (int iter = 1; iter <= K; iter++) {
        b2.x = ((double)rand() / RAND_MAX) * D;
        b2.y = ((double)rand() / RAND_MAX) * D;

        if (iter > T) jammed = 1;

        fprintf(log2, "--- Iteration %d: B at (%.2f, %.2f) | Jammed: %s ---\n",
                iter, b2.x, b2.y, jammed ? "YES" : "NO");

        int hitCount = 0;
        int localBSunkBy = -1;
        double localTotalTime = 0.0;

        for (int i = 0; i < N && localBSunkBy == -1; i++) {
            if (escorts2[i].destroyed) continue;

            if (isInRange(escorts2[i].x, escorts2[i].y, b2.x, b2.y, escorts2[i].vMax)) {
                localBSunkBy = escorts2[i].id;
                b2.destroyed = 1;
                break;
            }

            int hitResult;
            if (jammed) {
                hitResult = isInRangeJammed(b2.x, b2.y, escorts2[i].x, escorts2[i].y, b2.vMax, thetaMin);
            } else {
                hitResult = isInRange(b2.x, b2.y, escorts2[i].x, escorts2[i].y, b2.vMax);
            }

            if (hitResult) {
                escorts2[i].destroyed = 1;
                double t = timeToHit(b2.vMax);
                if (t > localTotalTime) localTotalTime = t;
                hitCount++;
                fprintf(log2, "  E%d hit at t=%.4f s\n", escorts2[i].id, t);
            }
        }

        if (b2.destroyed) {
            fprintf(log2, "Battleship SUNK by E%d\n\n", localBSunkBy);
            break;
        } else {
            fprintf(log2, "B survives. E ships hit: %d\n\n", hitCount);
        }
    }
    fclose(log2);
    printf("Simulation 2 done. Check simulation2_log.txt\n");

    saveFinalResults("final_results.txt", &b, escorts, N, bSunkBy, 0, hitIds, hitTimes, totalTime);

    return 0;
}
