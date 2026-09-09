#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ships.h"
#include "setup.h"
#include "physics.h"
#include "fileio.h"
#include "simulate.h"
#include "strategy.h"

int main() {
    srand(time(NULL));

    double D = 1000.0;
    int N = 5;
    double vMaxB = 800.0;
    double TB = 2.5;          // battleship firing interval (seconds)
    double simEndTime = 30.0; // stop the simulation after this much time

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    FILE *log = fopen("part2b_log.txt", "w");
    fprintf(log, "=== Part 2-B: Continuous Firing for Escort Ships ===\n");
    fprintf(log, "Battleship firing interval (TB): %.2f s\n\n", TB);

    // Sort E ships by distance so B attacks the closest one first
    sortByDistance(escorts, N, b.x, b.y);

    fprintf(log, "Attack order for B (closest to farthest):\n");
    for (int i = 0; i < N; i++) {
        double d = distance(b.x, b.y, escorts[i].x, escorts[i].y);
        fprintf(log, "  %d. E%d (Type %c) - distance: %.2f - TE: %.2f\n",
                i + 1, escorts[i].id, escorts[i].type, d, escorts[i].firingInterval);
    }
    fprintf(log, "\n");

    // Track each E ship's next fire time separately, since TE differs per type
    double nextFireTimeE[100];
    for (int i = 0; i < N; i++) {
        nextFireTimeE[i] = 0.0;
    }

    double nextFireTimeB = 0.0;
    int bAttackIndex = 0;   // next E ship in B's attack order
    double bDamage = 0.0;
    int hitCount = 0;

    fprintf(log, "--- Simulation timeline ---\n");

    double currentTime = 0.0;
    double timeStep = 0.5;  // small time step to advance the simulation

    while (currentTime <= simEndTime && !b.destroyed) {

        // Each alive E ship fires again once its own TE interval has passed
        for (int i = 0; i < N; i++) {
            if (escorts[i].destroyed) continue;

            if (currentTime >= nextFireTimeE[i]) {
                if (isInRange(escorts[i].x, escorts[i].y, b.x, b.y, escorts[i].vMax)) {
                    bDamage += escorts[i].impactPower;
                    fprintf(log, "Time %.2f s: E%d fires and hits B. Cumulative damage: %.2f%%\n",
                            currentTime, escorts[i].id, bDamage * 100);

                    if (bDamage >= 1.0) {
                        b.destroyed = 1;
                        fprintf(log, "Battleship DESTROYED at time %.2f s\n", currentTime);
                        break;
                    }
                }
                nextFireTimeE[i] = currentTime + escorts[i].firingInterval;
            }
        }

        if (b.destroyed) break;

        // B fires at its next target in the attack order once TB has passed
        if (currentTime >= nextFireTimeB && bAttackIndex < N) {
            // Move to the next E ship still alive in the attack order
            while (bAttackIndex < N && escorts[bAttackIndex].destroyed) {
                bAttackIndex++;
            }

            if (bAttackIndex < N) {
                EscortShip *target = &escorts[bAttackIndex];
                if (isInRange(b.x, b.y, target->x, target->y, b.vMax)) {
                    target->destroyed = 1;
                    hitCount++;
                    fprintf(log, "Time %.2f s: B fires and destroys E%d\n", currentTime, target->id);
                } else {
                    fprintf(log, "Time %.2f s: B fires at E%d but misses (out of range)\n", currentTime, target->id);
                }
                bAttackIndex++;
                nextFireTimeB = currentTime + TB;
            }
        }

        currentTime += timeStep;
    }

    fprintf(log, "\n=== Summary ===\n");
    if (b.destroyed) {
        fprintf(log, "Battleship destroyed. Total E ships destroyed by B: %d\n", hitCount);
    } else {
        fprintf(log, "Battleship survived until simulation ended.\n");
        fprintf(log, "Total E ships destroyed by B: %d\n", hitCount);
        fprintf(log, "Final damage on B: %.2f%%\n", bDamage * 100);
    }

    fclose(log);
    printf("Part 2-B simulation done. Check part2b_log.txt\n");

    return 0;
}
