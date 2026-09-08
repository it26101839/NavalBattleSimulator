#include <stdio.h>
#include "ships.h"

void saveInitialConditions(const char *filename, Battleship *b, EscortShip ships[], int n, double D) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(fp, "=== Initial Battlefield Conditions ===\n");
    fprintf(fp, "Canvas size: %.2f x %.2f\n\n", D, D);
    fprintf(fp, "Battleship: Type=%c Pos=(%.2f,%.2f) Vmax=%.2f\n\n",
            b->type, b->x, b->y, b->vMax);

    fprintf(fp, "Escort Ships:\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "E%d | Type: %c | Pos: (%.2f, %.2f) | Vmax: %.2f | Vmin: %.2f | AngleMin: %.2f | AngleMax: %.2f | Impact: %.2f\n",
                ships[i].id, ships[i].type, ships[i].x, ships[i].y,
                ships[i].vMax, ships[i].vMin, ships[i].angleMin, ships[i].angleMax,
                ships[i].impactPower);
    }

    fclose(fp);
}

void saveFinalResults(const char *filename, Battleship *b, EscortShip ships[], int n,
                       int bSunkBy, int hitCount, int hitIds[], double hitTimes[], double totalTime) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(fp, "=== Final Battlefield Results ===\n\n");

    if (b->destroyed) {
        fprintf(fp, "Battleship SUNK by E%d\n\n", bSunkBy);
    } else {
        fprintf(fp, "Battleship survived.\n");
        fprintf(fp, "Total E ships hit by B: %d\n", hitCount);
        fprintf(fp, "Total time taken to end battle: %.4f seconds\n\n", totalTime);

        fprintf(fp, "Details of E ships hit by B:\n");
        for (int i = 0; i < hitCount; i++) {
            fprintf(fp, "  E%d | Time to hit: %.4f s\n", hitIds[i], hitTimes[i]);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "Final state of all Escort Ships:\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "E%d | Type: %c | Pos: (%.2f, %.2f) | Status: %s\n",
                ships[i].id, ships[i].type, ships[i].x, ships[i].y,
                ships[i].destroyed ? "DESTROYED" : "ALIVE");
    }

    fprintf(fp, "\nBattleship final status: %s\n",
            b->destroyed ? "DESTROYED" : "ALIVE");

    fclose(fp);
}
