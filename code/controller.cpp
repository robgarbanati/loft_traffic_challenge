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

//===============================================================================================//
//===================================== Defines and Typedefs ====================================//
//===============================================================================================//
// Helpful error handling macro.
#define CHECK(arg, fmt, ...) {if(!(arg)) {printf("ERROR: " fmt "\n", ##__VA_ARGS__); goto error; } }

//===============================================================================================//
//===================================== Private Variables =======================================//
//===============================================================================================//

//static lane_t init = {
    //"INIT",    // .name
    //0,           // .min_signal_time
    //0,           // .max_signal_time
    //{0, 0},       // .sensor_indices
    //{0, 0},       // .signal_indices
    //NULL          // .next_lane
//};
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
}

static void turn_lane_green(const lane_t *current_lane, SignalState *signals) {
    int signal0 = current_lane->signal_indices[0];
    int signal1 = current_lane->signal_indices[1];

    signals[signal0] = SignalState::GREEN;
    signals[signal1] = SignalState::GREEN;
}

static void switch_to_lane(traffic_state_t *traffic_state, const lane_t *new_lane, int sim_time) {
    turn_lane_red(traffic_state->current_lane, traffic_state->signal_data);

    traffic_state->current_lane = new_lane;

    turn_lane_green(traffic_state->current_lane, traffic_state->signal_data);

    traffic_state->lane_start_time = sim_time;

    printf("Switched to lane %s at time %d. min: %d, max: %d\n",
            traffic_state->current_lane->name, sim_time,
            traffic_state->current_lane->min_signal_time,
            traffic_state->current_lane->max_signal_time);
}

static void controller_init(void) {
    ns_turn.next_lane = &ns_through;
    ns_through.next_lane = &ew_turn;
    ew_turn.next_lane = &ew_through;
    ew_through.next_lane = &ns_turn;
}

static const lane_t *find_next_lane_with_set_sensors(traffic_state_t *traffic_state, int sim_time) {
    const lane_t *new_lane = traffic_state->current_lane->next_lane;

    // Search through linked-list for next lane with set sensors.
    while(1) {
        //CHECK(new_lane != traffic_state->current_lane, "controller looped through all lanes and didn't find any with sensors set.");
        
        SensorState sensor0 = traffic_state->sensor_data[new_lane->sensor_indices[0]];
        SensorState sensor1 = traffic_state->sensor_data[new_lane->sensor_indices[1]];
        bool both_sensors_are_clear = ( (sensor0 == SensorState::CLEAR) && 
                                        (sensor1 == SensorState::CLEAR) );

        if(both_sensors_are_clear) {
            new_lane = new_lane->next_lane;
        } else {
            //printf("found lane\n");
            return new_lane;
        }
    }
}

static bool are_all_sensors_clear(traffic_state_t *traffic_state) {
    // look for any SET sensors in sensor_data
    for(int i=0; i<traffic_state->sensors_size; i++) {
        if(traffic_state->sensor_data[i] == SensorState::SET) {
            return false;
        }
    }
    // couldn't find any SET sensors, so return true.
    return true;
}


//===============================================================================================//
//====================================== Public Functions =======================================//
//===============================================================================================//

// TODO check if sensors are initialized.
void traffic_init(traffic_state_t *traffic_state, int sim_time) {
    controller_init();

    for(int i=0; i<traffic_state->signals_size; i++) {
        traffic_state->signal_data[i] = SignalState::RED;
    }

    switch_to_lane(traffic_state, &ns_turn, sim_time);
}

// Level 1: Follow traffic order.
// Level 2: Follow traffic order, skipping if sensor not present.
// Level 3: Follow traffic order, skipping if sensor not present, extending if sensor still present.
// Level 4: try to do something w.r.t. upper bound?
// Stateless!
void controller_update(traffic_state_t *traffic_state, int sim_time) {
    // Extract info from struct.
    const lane_t *current_lane = traffic_state->current_lane;
    SensorState sensor0 = traffic_state->sensor_data[current_lane->sensor_indices[0]];
    SensorState sensor1 = traffic_state->sensor_data[current_lane->sensor_indices[1]];
    int lane_start_time = traffic_state->lane_start_time;
    
    // compute various lane switching indicators.
    bool max_time_elapsed = ((sim_time - lane_start_time) >= current_lane->max_signal_time);
    bool min_time_elapsed = ((sim_time - lane_start_time) >= current_lane->min_signal_time);
    bool both_sensors_are_clear = ( (sensor0 == SensorState::CLEAR) && 
                                    (sensor1 == SensorState::CLEAR) );
    
    //printf("sim_time: %d. lane_start_time: %d. min: %d. max: %d. bothclear? %d\n", 
                //sim_time, lane_start_time, current_lane->min_signal_time, current_lane->max_signal_time, both_sensors_are_clear);

    // Is it time to switch lanes?
    if( max_time_elapsed || (min_time_elapsed && both_sensors_are_clear) ) {
        // if all sensors are clear, favor ns_through
        if(are_all_sensors_clear(traffic_state)) {
            switch_to_lane(traffic_state, &ns_through, sim_time);

        } else { // otherwise, seek next lane in rotation with set sensors.
            const lane_t *new_lane = find_next_lane_with_set_sensors(traffic_state, sim_time);

            // but don't bother 'switching' to same lane we're already in.
            if(new_lane != traffic_state->current_lane) {
                switch_to_lane(traffic_state, new_lane, sim_time);
            }
        }
    }
error:
    return;
} 

