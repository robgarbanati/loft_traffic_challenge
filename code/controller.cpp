// controller will receive sensors and time and determine what color to set each lane signal.
//
// The intersection should follow this cycle order.
//      1. N and S turning traffic
//      2. N and S through traffic
//      3. E and W turning traffic
//      4. E and W through traffic

// Traffic Pattern      Min Active      Max Active
// N and S turning      10s             60s 
// N and S through      30s             120s
// E and W turning      10s             30s 
// E and W through      30s             60s

// TODO determine max reasonable wait-time
// TODO find a way to test all this

//===============================================================================================//
//=========================================== Includes ==========================================//
//===============================================================================================//
#include "controller.hpp"

extern "C" {
//===============================================================================================//
//===================================== Private Variables =======================================//
//===============================================================================================//

static lane_t ns_turn = {
    "NS_TURN",    // .name
    10,           // .min_signal_time
    60,           // .max_signal_time
    {1, 3},       // .sensor_indices
    {1, 3},       // .signal_indices
    NULL          // .next_lane
};
static lane_t ns_through = {
    "NS_THROUGH", // .name
    30,           // .min_signal_time
    120,          // .max_signal_time
    {0, 2},       // .sensor_indices
    {0, 2},       // .signal_indices
    NULL          // .next_lane
};

static lane_t ew_turn = {
    "EW_TURN",    // .name
    10,           // .min_signal_time
    30,           // .max_signal_time
    {5, 7},       // .sensor_indices
    {5, 7},       // .signal_indices
    NULL          // .next_lane
};
static lane_t ew_through = {
    "EW_THROUGH", // .name
    30,           // .min_signal_time
    60,           // .max_signal_time
    {4, 6},       // .sensor_indices
    {4, 6},       // .signal_indices
    NULL          // .next_lane
};

//===============================================================================================//
//===================================== Private Functions =======================================//
//===============================================================================================//

static void turn_lane_red(const lane_t *current_lane, SignalState *signals) {
    int signal0 = current_lane->signal_indices[0];
    int signal1 = current_lane->signal_indices[1];

    signals[signal0] = SignalState::RED;
    signals[signal1] = SignalState::RED;
    printf("turn red: %d %d\n", signal0, signal1);
}

static void turn_lane_green(const lane_t *current_lane, SignalState *signals) {
    int signal0 = current_lane->signal_indices[0];
    int signal1 = current_lane->signal_indices[1];

    signals[signal0] = SignalState::GREEN;
    signals[signal1] = SignalState::GREEN;
    printf("turn green: %d %d\n", signal0, signal1);
}

static void update_lane(const lane_t **current_lane_ptr, int sim_time,
                        const SensorState sensor0, const SensorState sensor1, 
                        SignalState *signals) {
    const lane_t *current_lane = *current_lane_ptr;
    static int lane_start_time = 0;
    
    bool max_time_elapsed = ((sim_time - lane_start_time) > current_lane->max_signal_time);
    bool min_time_elapsed = ((sim_time - lane_start_time) > current_lane->min_signal_time);
    bool both_sensors_are_clear = ( (sensor0 == SensorState::CLEAR) && 
                                    (sensor1 == SensorState::CLEAR) );

    // Has max time in current signal elapsed?
    if( max_time_elapsed || (min_time_elapsed && both_sensors_are_clear) ) {
        turn_lane_red(current_lane, signals);

        *current_lane_ptr = current_lane->next_lane; // TODO make this a function that does more advanced switching logic.
        current_lane = *current_lane_ptr;
        turn_lane_green(current_lane, signals);

        lane_start_time = sim_time;
        printf("Switched to lane %s at time %d. min: %d, max: %d\n", current_lane->name, sim_time, current_lane->min_signal_time, current_lane->max_signal_time);

    }
}

static void controller_init(void) {
    ns_turn.next_lane = &ns_through;
    ns_through.next_lane = &ew_turn;
    ew_turn.next_lane = &ew_through;
    ew_through.next_lane = &ns_turn;
}

//===============================================================================================//
//====================================== Public Functions =======================================//
//===============================================================================================//

// TODO check if sensors are initialized.
void traffic_init(traffic_state_t *traffic_state) {
    controller_init();
    traffic_state->current_lane = &ns_turn;
    for(int i=0; i<traffic_state->signals_size; i++) {
        traffic_state->signals[i] = SignalState::RED;
    }
    turn_lane_green(traffic_state->current_lane, traffic_state->signals);
}

// Level 1: Follow traffic order.
// Level 2: Follow traffic order, skipping if sensor not present.
// Level 3: Follow traffic order, skipping if sensor not present, extending if sensor still present.
// Level 4: try to do something w.r.t. upper bound?
// Stateless!
void controller_update(traffic_state_t *traffic_state) {

    // Extract info from struct.
    const lane_t *current_lane = traffic_state->current_lane;
    SensorState sensor0 = traffic_state->sensors[current_lane->sensor_indices[0]];
    SensorState sensor1 = traffic_state->sensors[current_lane->sensor_indices[1]];
    int sim_time = traffic_state->sim_time;
    int lane_start_time = traffic_state->lane_start_time;
    
    // compute various lane switching indicators.
    bool max_time_elapsed = ((sim_time - lane_start_time) > current_lane->max_signal_time);
    bool min_time_elapsed = ((sim_time - lane_start_time) > current_lane->min_signal_time);
    bool both_sensors_are_clear = ( (sensor0 == SensorState::CLEAR) && 
                                    (sensor1 == SensorState::CLEAR) );

    // Has max time in current signal elapsed?
    if( max_time_elapsed || (min_time_elapsed && both_sensors_are_clear) ) {
        turn_lane_red(traffic_state->current_lane, traffic_state->signals);

        traffic_state->current_lane = current_lane->next_lane; // TODO make this a function that does more advanced switching logic.
        turn_lane_green(traffic_state->current_lane, traffic_state->signals);

        traffic_state->lane_start_time = sim_time;
        printf("Switched to lane %s at time %d. min: %d, max: %d\n", current_lane->name, sim_time, current_lane->min_signal_time, current_lane->max_signal_time);
    }
} 

} // extern "C"
