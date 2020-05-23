#include "controller.hpp"

SignalState *controller_update(const SensorState *sensors, int sensors_size, SignalState *signals, int signals_size) {
    printf("in controller_update.\n");
    for(int i=0;i<sensors_size;i++) {
        printf("%d ", (int) sensors[i]);
    }
    puts("");

    for(int i=0; i<signals_size; i++) {
        signals[i] = SignalState::GREEN;
    }
    
    return NULL;
} 
