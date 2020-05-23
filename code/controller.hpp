#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <stdio.h>
#include "simulator.hpp"
#include "clock.hpp"

typedef struct _lane_t {
    const char name[11];
    unsigned int min_signal_time;
    unsigned int max_signal_time;
    unsigned int sensor_indices[2];
    const struct _lane_t *next_lane;
} lane_t;

void controller_init(void);
SignalState *controller_update(const SensorState *sensors, int sensors_size, SignalState *signals, int signals_size, int sim_time);

#endif // CONTROLLER_HPP
