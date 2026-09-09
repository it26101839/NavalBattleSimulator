#include "ships.h"
#include "physics.h"

// Simulates one attack attempt (same logic as Part 1-A)
// Returns hitCount
int simulateAttack(Battleship *b, EscortShip escorts[], int n,
                    int hitIds[], double hitTimes[], double *totalTime,
                    int *bSunkBy) {
    int hitCount = 0;
    *bSunkBy = -1;
    *totalTime = 0.0;
    b->destroyed = 0;

    for (int i = 0; i < n && *bSunkBy == -1; i++) {
        if (escorts[i].destroyed) continue;  // already destroyed ships are skipped

        if (isInRange(escorts[i].x, escorts[i].y, b->x, b->y, escorts[i].vMax)) {
            *bSunkBy = escorts[i].id;
            b->destroyed = 1;
            break;
        }
        if (isInRange(b->x, b->y, escorts[i].x, escorts[i].y, b->vMax)) {
            escorts[i].destroyed = 1;
            double t = timeToHit(b->vMax);
            hitIds[hitCount] = escorts[i].id;
            hitTimes[hitCount] = t;
            if (t > *totalTime) *totalTime = t;
            hitCount++;
        }
    }

    return hitCount;
}
