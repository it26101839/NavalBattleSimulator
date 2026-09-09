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
    int T = 2;                // iteration after which gun jams (Simulation 2)
    double thetaMin = 15.0;   // minimum angle allowed after jam

    EscortShip escortsOriginal[N];
    initEscortShips(escortsOriginal, N, D, vMaxB);

    Battleship bOriginal;
    initBattleship(&bOriginal, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &bOriginal, escortsOriginal, N, D);

    int hitIds[100];
    double hitTimes[100];
    double totalTime;

    // ============ SIMULATION 1 (with % damage) ============
    FILE *log1 = fopen("simulation1_log.txt", "w");
    fprintf(log1, "=== Part 1-C Simulation 1: Battleship Moving Path (with %% damage) ===\n\n");

    EscortShip escorts1[N];
    for (int i = 0; i < N; i++) escorts1[i] = escortsOriginal[i];

    Battleship b1 = bOriginal;
    double bDamage1 = 0.0;

    for (int iter = 1; iter <= K; iter++) {
        b1.x = ((double)rand() / RAND_MAX) * D;
        b1.y = ((double)rand() / RAND_MAX) * D;

        fprintf(log1, "--- Iteration %d: B at (%.2f, %.2f) ---\n", iter, b1.x, b1.y);

        int hitCount = simulateAttackPercent(&b1, escorts1, N, hitIds, hitTimes, &totalTime, &bDamage1);

        fprintf(log1, "Cumulative damage on B: %.2f%%\n", bDamage1 * 100);

        for (int i = 0; i < hitCount; i++) {
            fprintf(log1, "  E%d hit at t=%.4f s\n", hitIds[i], hitTimes[i]);
        }

        if (b1.destroyed) {
            fprintf(log1, "Battleship DESTROYED (cumulative damage reached 100%%)\n\n");
            break;
        }
        fprintf(log1, "\n");
    }
    if (!b1.destroyed) {
        fprintf(log1, "Battleship survived all iterations. Final damage: %.2f%%\n", bDamage1 * 100);
    }
    fclose(log1);
    printf("Simulation 1 done. Check simulation1_log.txt\n");

    // ============ SIMULATION 2 (Gun Jam + % damage) ============
    FILE *log2 = fopen("simulation2_log.txt", "w");
    fprintf(log2, "=== Part 1-C Simulation 2: Gun Jam after iteration %d (with %% damage) ===\n\n", T);

    EscortShip escorts2[N];
    for (int i = 0; i < N; i++) escorts2[i] = escortsOriginal[i];

    Battleship b2 = bOriginal;
    double bDamage2 = 0.0;
    int jammed = 0;

    for (int iter = 1; iter <= K; iter++) {
        b2.x = ((double)rand() / RAND_MAX) * D;
        b2.y = ((double)rand() / RAND_MAX) * D;

        if (iter > T) jammed = 1;

        fprintf(log2, "--- Iteration %d: B at (%.2f, %.2f) | Jammed: %s ---\n",
                iter, b2.x, b2.y, jammed ? "YES" : "NO");

        int hitCount = 0;
        double localTotalTime = 0.0;

        for (int i = 0; i < N; i++) {
            if (escorts2[i].destroyed) continue;

            if (isInRange(escorts2[i].x, escorts2[i].y, b2.x, b2.y, escorts2[i].vMax)) {
                bDamage2 += escorts2[i].impactPower;
                if (bDamage2 >= 1.0) {
                    b2.destroyed = 1;
                }
            }

            int hitResult;
            if (!b2.destroyed) {
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

            if (b2.destroyed) break;
        }

        fprintf(log2, "Cumulative damage on B: %.2f%%\n", bDamage2 * 100);

        if (b2.destroyed) {
            fprintf(log2, "Battleship DESTROYED (cumulative damage reached 100%%)\n\n");
            break;
        }
        fprintf(log2, "\n");
    }
    if (!b2.destroyed) {
        fprintf(log2, "Battleship survived all iterations. Final damage: %.2f%%\n", bDamage2 * 100);
    }
    fclose(log2);
    printf("Simulation 2 done. Check simulation2_log.txt\n");

    return 0;
}
