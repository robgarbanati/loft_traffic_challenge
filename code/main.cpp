#include "simulator.hpp"
#include "controller.hpp"

using SS = SensorState;

static constexpr Clock::Time TIME_STEP = 10; ///< advance simulator by 10s for each step

/// At T+0, non-stop traffic in all directions. continues for 5 minutes
static const Scenario SCENARIO_1 = 
{   //            N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,  300,  { SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET,   SS::SET   }}
};

/// at T+0, there is  N-W and S-E traffic
/// at T+10, an infinite line of vehicles pulls up to the N-N sensor
/// at T+20, all N-W and S-E traffic stops
static const Scenario SCENARIO_2 = 
{   //             N-N        N-W        S-S        S-E        E-E        E-N        W-W        W-S
    { 0,   10,   { SS::CLEAR, SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 10,  20,   { SS::SET,   SS::SET,   SS::CLEAR, SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
    { 20,  300,  { SS::SET,   SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR, SS::CLEAR }},
};

int main
(
    int argc, 
    char const *argv[]
)
{
    Simulator simulator(SCENARIO_2);
    auto &clock = simulator.clock();
    auto &sensors = simulator.sensors();

    TrafficSignals signals;

    traffic_state_t traffic_state;
    traffic_state.sensor_data = sensors.data();
    traffic_state.sensors_size = sensors.size();
    traffic_state.signal_data = signals.data();
    traffic_state.signals_size = signals.size();
    traffic_init(&traffic_state, clock.now());


    std::cout << simulator.BANNER << std::endl;

    for(;;)
    {
        if (simulator.done())
        {
            break;
        }

        // Pass traffic state into controller. controller will update signals array via 
        // traffic_state.signal_data ptr
        controller_update(&traffic_state, clock.now());

        // Pass signals into simulator.
        simulator.update_lane_signals(signals);

        // Let simulator print simulator state.
        std::cout << simulator << std::endl;
        simulator.advance(TIME_STEP);

        // TODO write tests for everything.
    }
}
