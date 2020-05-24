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
    unsigned int signal_indices[2];
    const struct _lane_t *next_lane;
} lane_t;

typedef struct _traffic_state_t {
    const lane_t *current_lane;
    int lane_start_time;
    const SensorState *sensor_data;
    int sensors_size;
    SignalState *signal_data;
    int signals_size;
    int sim_time;
} traffic_state_t;

void traffic_init(traffic_state_t *traffic_state, int sim_time);
void controller_update(traffic_state_t *traffic_state, int sim_time);

#endif // CONTROLLER_HPP
