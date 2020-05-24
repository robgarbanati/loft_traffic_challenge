#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <stdio.h>
#include "simulator.hpp"
#include "clock.hpp"


extern "C" {
typedef struct _lane_t {
    const char name[11];
    unsigned int min_signal_time;
    unsigned int max_signal_time;
    unsigned int sensor_indices[2];
    unsigned int signal_indices[2];
    const struct _lane_t *next_lane;
} lane_t;

typedef struct _traffic_state_t {
    const lane_t *current_lane;
    int lane_start_time;
    const SensorState *sensors;
    int sensors_size;
    SignalState *signals;
    int signals_size;
    int sim_time;
} traffic_state_t;

void controller_init(traffic_state_t *traffic_state);
void controller_update(traffic_state_t traffic_state);

} // extern "C"
#endif // CONTROLLER_HPP
