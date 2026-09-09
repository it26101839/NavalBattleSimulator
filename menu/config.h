#ifndef CONFIG_H
#define CONFIG_H

#define GRAVITY 9.81
#define MAX_ESCORT_TYPES 5

static const double IMPACT_POWER[5] = {0.08, 0.06, 0.07, 0.05, 0.04};
static const double ANGLE_RANGE[5]  = {20, 30, 25, 50, 70};

// TE values - firing interval (seconds) per escort ship type
static const double TE_VALUES[5] = {3.0, 1.5, 2.0, 4.0, 5.0};

#endif
