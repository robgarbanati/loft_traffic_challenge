#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "simulator.hpp"
#include <stdio.h>

SignalState *controller_update(const SensorState *sensors, int sensors_size, SignalState *signals, int signals_size);

#endif // CONTROLLER_HPP
