#include "ships.h"
#include "physics.h"

// Simple strategy: closest E ship ekata kalin attack karanawa
// Bubble sort use karala escorts array eka distance anuwa sort karanawa
void sortByDistance(EscortShip escorts[], int n, double bx, double by) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            double d1 = distance(bx, by, escorts[j].x, escorts[j].y);
            double d2 = distance(bx, by, escorts[j + 1].x, escorts[j + 1].y);
            if (d1 > d2) {
                EscortShip temp = escorts[j];
                escorts[j] = escorts[j + 1];
                escorts[j + 1] = temp;
            }
        }
    }
}
