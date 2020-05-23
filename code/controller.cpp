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
//===================================== Private Variables =======================================//
//===============================================================================================//

static lane_t ns_turn = {
    "NS_TURN",    // .name
    10,           // .min_signal_time
    60,           // .max_signal_time
    {0, 1},       // .sensor_indices
    NULL          // .next_lane
};
static lane_t ns_through = {
    "NS_THROUGH", // .name
    30,           // .min_signal_time
    120,          // .max_signal_time
    {0, 1},       // .sensor_indices
    NULL          // .next_lane
};

static lane_t ew_turn = {
    "EW_TURN",    // .name
    10,           // .min_signal_time
    30,           // .max_signal_time
    {0, 1},       // .sensor_indices
    NULL          // .next_lane
};
static lane_t ew_through = {
    "EW_THROUGH", // .name
    30,           // .min_signal_time
    60,           // .max_signal_time
    {0, 1},       // .sensor_indices
    NULL          // .next_lane
};


//===============================================================================================//
//===================================== Private Functions =======================================//
//===============================================================================================//

static void update_lane(const lane_t *current_lane, int sim_time,
                        const SensorState sensor0, const SensorState sensor1) {
    static int lane_start_time = 0;
    
    bool max_time_elapsed = ((sim_time - lane_start_time) > current_lane->max_signal_time);
    bool min_time_elapsed = ((sim_time - lane_start_time) > current_lane->min_signal_time);
    bool both_sensors_are_clear = ( (sensor0 == SensorState::CLEAR) && 
                                    (sensor1 == SensorState::CLEAR) );

    // Has max time in current signal elapsed?
    if( max_time_elapsed || (min_time_elapsed && both_sensors_are_clear) ) {
        current_lane = current_lane->next_lane; // TODO make this a function that does more advanced switching logic.
        printf("Switched to new lane: %s\n", current_lane->name);
    }

}

//===============================================================================================//
//====================================== Public Functions =======================================//
//===============================================================================================//

void controller_init(void) {
    ns_turn.next_lane = &ns_through;
    ns_through.next_lane = &ew_turn;
    ew_turn.next_lane = &ew_through;
    ew_through.next_lane = &ns_turn;
}

// Level 1: Follow traffic order.
// Level 2: Follow traffic order, skipping if sensor not present.
// Level 3: Follow traffic order, skipping if sensor not present, extending if sensor still present.
// Level 4: try to do something w.r.t. upper bound?
SignalState *controller_update( const SensorState *sensors, int sensors_size, 
                                SignalState *signals, int signals_size,
                                int sim_time) {
    static const lane_t *current_lane = &ns_turn;

    printf("in controller_update.\n");
    for(int i=0;i<sensors_size;i++) {
        printf("%d ", (int) sensors[i]);
    }
    puts("");

    // Get relevant sensors for this lane.
    SensorState sensor0 = sensors[current_lane->sensor_indices[0]];
    SensorState sensor1 = sensors[current_lane->sensor_indices[1]];

    // Decide if it's time to update signals.
    update_lane(current_lane, sim_time, sensor0, sensor1);

    // Update signals.
    for(int i=0; i<signals_size; i++) {
        signals[i] = SignalState::GREEN;
    }


    return NULL;
} 
