#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "ships.h"
#include "setup.h"
#include "physics.h"
#include "fileio.h"
#include "simulate.h"
#include "strategy.h"

void printInstructions() {
    printf("\n=== Instructions ===\n");
    printf("This simulator models a battle between a Battleship (B) and\n");
    printf("several Escort Ships (E) on a 2D battlefield.\n\n");
    printf("- B and E ships fire shells that follow projectile motion.\n");
    printf("- Each E ship type has different velocity, angle range, and impact power.\n");
    printf("- B tries to destroy as many E ships as possible while surviving.\n");
    printf("- Results are saved to text files after each simulation.\n\n");
    printf("Menu Options:\n");
    printf("  1. Start Simulation - set up and run a new battle simulation\n");
    printf("  2. View Instructions - this screen\n");
    printf("  3. Simulation Statistics - view results from the last simulation\n");
    printf("  4. Exit - close the program\n\n");
}

void showStatistics() {
    printf("\n=== Simulation Statistics ===\n");

    FILE *fp = fopen("initial_conditions.txt", "r");
    if (fp == NULL) {
        printf("No simulation has been run yet.\n");
        return;
    }

    char line[256];
    printf("\n--- initial_conditions.txt ---\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);

    fp = fopen("part2b_log.txt", "r");
    if (fp != NULL) {
        printf("\n--- part2b_log.txt ---\n");
        while (fgets(line, sizeof(line), fp)) {
            printf("%s", line);
        }
        fclose(fp);
    }
}

void runSimulation() {
    srand(time(NULL));

    double D = 1000.0;
    int N = 5;
    double vMaxB = 800.0;
    double TB = 2.5;
    double simEndTime = 30.0;

    printf("\nEnter canvas size D: ");
    scanf("%lf", &D);
    printf("Enter number of escort ships N: ");
    scanf("%d", &N);
    printf("Enter battleship max velocity: ");
    scanf("%lf", &vMaxB);

    EscortShip escorts[N];
    initEscortShips(escorts, N, D, vMaxB);

    Battleship b;
    initBattleship(&b, 'U', D / 2, D / 2, vMaxB);

    saveInitialConditions("initial_conditions.txt", &b, escorts, N, D);

    FILE *log = fopen("part2b_log.txt", "w");
    fprintf(log, "=== Simulation: Continuous Firing for Escort Ships ===\n");
    fprintf(log, "Battleship firing interval (TB): %.2f s\n\n", TB);

    sortByDistance(escorts, N, b.x, b.y);

    fprintf(log, "Attack order for B (closest to farthest):\n");
    for (int i = 0; i < N; i++) {
        double d = distance(b.x, b.y, escorts[i].x, escorts[i].y);
        fprintf(log, "  %d. E%d (Type %c) - distance: %.2f - TE: %.2f\n",
                i + 1, escorts[i].id, escorts[i].type, d, escorts[i].firingInterval);
    }
    fprintf(log, "\n");

    double nextFireTimeE[100];
    for (int i = 0; i < N; i++) {
        nextFireTimeE[i] = 0.0;
    }

    double nextFireTimeB = 0.0;
    int bAttackIndex = 0;
    double bDamage = 0.0;
    int hitCount = 0;

    fprintf(log, "--- Simulation timeline ---\n");

    double currentTime = 0.0;
    double timeStep = 0.5;

    while (currentTime <= simEndTime && !b.destroyed) {
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

        if (currentTime >= nextFireTimeB && bAttackIndex < N) {
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
        printf("\nBattleship was DESTROYED. Total E ships destroyed by B: %d\n", hitCount);
    } else {
        fprintf(log, "Battleship survived until simulation ended.\n");
        fprintf(log, "Total E ships destroyed by B: %d\n", hitCount);
        fprintf(log, "Final damage on B: %.2f%%\n", bDamage * 100);
        printf("\nBattleship SURVIVED. E ships destroyed: %d, Final damage: %.2f%%\n", hitCount, bDamage * 100);
    }

    fclose(log);
    printf("Results saved to initial_conditions.txt and part2b_log.txt\n");
}

int main() {
    int choice;

    do {
        printf("\n=== Naval Battle Simulator - Main Menu ===\n");
        printf("1. Start Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Simulation Statistics\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                runSimulation();
                break;
            case 2:
                printInstructions();
                break;
            case 3:
                showStatistics();
                break;
            case 4:
                printf("\nExiting simulator. Goodbye!\n");
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}
