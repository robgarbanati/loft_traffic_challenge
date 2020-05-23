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
    Simulator simulator(SCENARIO_1);
    auto &clock = simulator.clock();
    auto &sensors = simulator.sensors();

    std::cout << simulator.BANNER << std::endl;

    for(;;)
    {
        if (simulator.done())
        {
            break;
        }

        // Extract array from sensors from simulator.
        const SensorState *c_sensors = sensors.data();

        // Pass sensors into controller. Get signals from controller.
        TrafficSignals testsignals;
        for (unsigned lane = 0; lane < Lane::COUNT; ++lane)
        {
            testsignals[lane] = SignalState::YELLOW;
        }
        SignalState *c_signals = testsignals.data();
        c_signals = controller_update(c_sensors, sensors.size(), c_signals, testsignals.size());

        // Convert c_signals to signals somehow.

        // Pass signals into simulator.
        simulator.update_lane_signals(testsignals);

        // Let simulator print simulator state.
        std::cout << simulator << std::endl;
        simulator.advance(TIME_STEP);

        // TODO write tests for everything.
    }
}
