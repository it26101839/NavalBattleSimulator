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
    double TB = 2.5;   // B ge firing interval eka (seconds), user input/random wenna puluwan

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    FILE *log = fopen("part2a_log.txt", "w");
    fprintf(log, "=== Part 2-A: Firing Interval + Attack Order Strategy ===\n");
    fprintf(log, "Battleship firing interval (TB): %.2f s\n\n", TB);

    // Strategy: B range ekata E ships close ewata anuwa sort karanawa
    // (closest ekata kalin attack karanawa - simple strategy)
    sortByDistance(escorts, N, b.x, b.y);

    fprintf(log, "Attack order (closest to farthest):\n");
    for (int i = 0; i < N; i++) {
        double d = distance(b.x, b.y, escorts[i].x, escorts[i].y);
        fprintf(log, "  %d. E%d (Type %c) - distance: %.2f\n", i + 1, escorts[i].id, escorts[i].type, d);
    }
    fprintf(log, "\n");

    double currentTime = 0.0;
    double bDamage = 0.0;
    int hitCount = 0;

    fprintf(log, "--- Simulation with firing sequence ---\n");

    for (int i = 0; i < N; i++) {
        if (escorts[i].destroyed) continue;

        // E ekak B ta hit karanawada balanawa (kalinma)
        if (isInRange(escorts[i].x, escorts[i].y, b.x, b.y, escorts[i].vMax)) {
            bDamage += escorts[i].impactPower;
            fprintf(log, "E%d hits B! Cumulative damage: %.2f%%\n", escorts[i].id, bDamage * 100);
            if (bDamage >= 1.0) {
                b.destroyed = 1;
                fprintf(log, "Battleship DESTROYED at time %.2f s\n", currentTime);
                break;
            }
        }

        // B eka attack order eken E ekata fire karanawa
        if (isInRange(b.x, b.y, escorts[i].x, escorts[i].y, b.vMax)) {
            escorts[i].destroyed = 1;
            hitCount++;
            fprintf(log, "Time %.2f s: B fires and destroys E%d\n", currentTime, escorts[i].id);
        } else {
            fprintf(log, "Time %.2f s: B fires at E%d but misses (out of range)\n", currentTime, escorts[i].id);
        }

        currentTime += TB;  // 襤ext shot ekata kalin interval eka enawa
    }

    fprintf(log, "\n=== Summary ===\n");
    if (b.destroyed) {
        fprintf(log, "Battleship destroyed. Total E ships destroyed by B: %d\n", hitCount);
    } else {
        fprintf(log, "Battleship survived. Total E ships destroyed: %d\n", hitCount);
        fprintf(log, "Final damage on B: %.2f%%\n", bDamage * 100);
        fprintf(log, "Total battle time: %.2f s\n", currentTime);
    }

    fclose(log);
    printf("Part 2-A simulation done. Check part2a_log.txt\n");

    return 0;
}
