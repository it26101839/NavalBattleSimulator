#include "ships.h"
#include "physics.h"

// Part 1-C: A single E ship attack only reduces B by a percentage
// bDamage is cumulative damage on B (0.0 to 1.0)
int simulateAttackPercent(Battleship *b, EscortShip escorts[], int n,
                           int hitIds[], double hitTimes[], double *totalTime,
                           double *bDamage) {
    int hitCount = 0;
    *totalTime = 0.0;

    for (int i = 0; i < n; i++) {
        if (escorts[i].destroyed) continue;

        if (isInRange(escorts[i].x, escorts[i].y, b->x, b->y, escorts[i].vMax)) {
            *bDamage += escorts[i].impactPower;
            if (*bDamage >= 1.0) {
                b->destroyed = 1;
            }
        }

        if (!b->destroyed && isInRange(b->x, b->y, escorts[i].x, escorts[i].y, b->vMax)) {
            escorts[i].destroyed = 1;
            double t = timeToHit(b->vMax);
            hitIds[hitCount] = escorts[i].id;
            hitTimes[hitCount] = t;
            if (t > *totalTime) *totalTime = t;
            hitCount++;
        }

        if (b->destroyed) break;
    }

    return hitCount;
}
