#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// ============ CONSTANTS ============
#define GRAVITY 9.81
#define MAX_ESCORT_TYPES 5

static const double IMPACT_POWER[5] = {0.08, 0.06, 0.07, 0.05, 0.04};
static const double ANGLE_RANGE[5]  = {20, 30, 25, 50, 70};
static const double TE_VALUES[5]    = {3.0, 1.5, 2.0, 4.0, 5.0};

// ============ STRUCTS ============
typedef struct {
    int id;
    char type;
    double x, y;
    double vMax, vMin;
    double angleMin, angleMax;
    double impactPower;
    double firingInterval;
    int destroyed;
} EscortShip;

typedef struct {
    char type;
    double x, y;
    double vMax;
    int destroyed;
} Battleship;

// ============ SETUP FUNCTIONS ============
void initEscortShips(EscortShip ships[], int n, double D, double vMaxB) {
    char typeChars[5] = {'A', 'B', 'C', 'D', 'E'};

    for (int i = 0; i < n; i++) {
        ships[i].id = i + 1;

        int typeIndex = rand() % 5;
        ships[i].type = typeChars[typeIndex];

        ships[i].x = ((double)rand() / RAND_MAX) * D;
        ships[i].y = ((double)rand() / RAND_MAX) * D;

        ships[i].impactPower = IMPACT_POWER[typeIndex];
        ships[i].firingInterval = TE_VALUES[typeIndex];

        ships[i].angleMin = ((double)rand() / RAND_MAX) * 30;
        ships[i].angleMax = ships[i].angleMin + ANGLE_RANGE[typeIndex];

        if (typeIndex == 0) {
            ships[i].vMax = 1.2 * vMaxB;
        } else {
            ships[i].vMax = ((double)rand() / RAND_MAX) * vMaxB;
        }
        ships[i].vMin = ((double)rand() / RAND_MAX) * ships[i].vMax;

        ships[i].destroyed = 0;
    }
}

void printEscortShips(EscortShip ships[], int n) {
    for (int i = 0; i < n; i++) {
        printf("E%d | Type: %c | Pos: (%.2f, %.2f) | Vmax: %.2f | Impact: %.2f | TE: %.2f\n",
               ships[i].id, ships[i].type, ships[i].x, ships[i].y,
               ships[i].vMax, ships[i].impactPower, ships[i].firingInterval);
    }
}

void initBattleship(Battleship *b, char type, double x, double y, double vMax) {
    b->type = type;
    b->x = x;
    b->y = y;
    b->vMax = vMax;
    b->destroyed = 0;
}

// ============ PHYSICS FUNCTIONS ============
double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double maxRange(double v) {
    double thetaRad = 45.0 * M_PI / 180.0;
    return (v * v * sin(2 * thetaRad)) / GRAVITY;
}

int isInRange(double shooterX, double shooterY, double targetX, double targetY, double v) {
    double d = distance(shooterX, shooterY, targetX, targetY);
    double range = maxRange(v);
    return (d <= range);
}

double timeToHit(double v) {
    double thetaRad = 45.0 * M_PI / 180.0;
    return (2 * v * sin(thetaRad)) / GRAVITY;
}

double maxRangeWithAngle(double v, double theta) {
    double thetaRad = theta * M_PI / 180.0;
    return (v * v * sin(2 * thetaRad)) / GRAVITY;
}

int isInRangeJammed(double shooterX, double shooterY, double targetX, double targetY,
                     double v, double thetaMin) {
    double bestTheta = (thetaMin <= 45.0) ? 45.0 : thetaMin;
    double d = distance(shooterX, shooterY, targetX, targetY);
    double range = maxRangeWithAngle(v, bestTheta);
    return (d <= range);
}

// ============ STRATEGY FUNCTION ============
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

// ============ FILE HANDLING FUNCTIONS ============
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

// ============ ATTACK SIMULATION (Percentage Damage) ============
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

// ============ MENU ACTIONS ============
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

    fp = fopen("battle_log.txt", "r");
    if (fp != NULL) {
        printf("\n--- battle_log.txt ---\n");
        while (fgets(line, sizeof(line), fp)) {
            printf("%s", line);
        }
        fclose(fp);
    }
}

void runSimulation() {
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

    FILE *log = fopen("battle_log.txt", "w");
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
    printf("Results saved to initial_conditions.txt and battle_log.txt\n");
}

// ============ MAIN MENU ============
int main() {
    srand(time(NULL));
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
