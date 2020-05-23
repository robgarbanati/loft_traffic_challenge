#include "controller.hpp"

// The intersection should follow this cycle order.
//      1. N and S turning traffic
//      2. N and S through traffic
//      3. E and W turning traffic
//      4. E and W through traffic

// Traffic Pattern      Min Active      Max Active
// N and S turning      10s             30s 
// N and S through      10s             30s
// E and W turning      60s             120s 
// E and W through      30s             60s

// TODO determine max reasonable wait-time
// TODO find a way to test all this

// Level 1: Follow traffic order.
// Level 2: Follow traffic order, skipping if sensor not present.
// Level 3: Follow traffic order, skipping if sensor not present, extending if sensor still present.
// Level 4: try to do something w.r.t. upper bound?
SignalState *controller_update(const SensorState *sensors, int sensors_size, SignalState *signals, int signals_size) {
    printf("in controller_update.\n");
    for(int i=0;i<sensors_size;i++) {
        printf("%d ", (int) sensors[i]);
    }
    puts("");

    // Update signals.
    for(int i=0; i<signals_size; i++) {
        signals[i] = SignalState::GREEN;
    }
    
    return NULL;
} 
