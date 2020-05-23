#include "simulator.hpp"

#include <algorithm>
#include <iomanip>

const std::string Simulator::BANNER = 
"  Time     N-N   N-W   S-S   S-E   E-E   E-N   W-W   W-S\n"
"==========================================================";

static const std::string& signal_string
(
    SignalState signal
)
{
    static const std::vector<std::string> STRINGS = 
    {
        "RED",
        "YLW",
        "GRN"
    };

    return STRINGS[static_cast<unsigned>(signal)];
}

static const std::string& sensor_string
(
    SensorState sensor
)
{
    static const std::vector<std::string> STRINGS = 
    {
        "SET",
        "CLEAR"
    };

    return STRINGS[static_cast<unsigned>(sensor)];
}

std::ostream& operator<<
(
    std::ostream& os, 
    const Simulator& simulator
)
{
    std::cout << "[" << std::setw(4) << simulator.clock_.now() << "s] ";
    for (unsigned lane = 0; lane < simulator.signals_.size(); lane++)
    {
        auto signal = simulator.signals_[lane];
        std::cout << " | " << signal_string(signal);
    }
    std::cout << " | ";

    std::cout << "[" << std::setw(4) << simulator.clock_.now() << "s] ";
    for (unsigned lane = 0; lane < simulator.sensors_.size(); lane++)
    {
        auto sensor = simulator.sensors_[lane];
        std::cout << " | " << sensor_string(sensor);
    }
    std::cout << " | ";
}

void Simulator::update_simulation
(
    void
)
{
    // find entry in scneario for current timestamp
    auto scenario_timeslice = 
        std::find_if(
            scenario_.begin(), scenario_.end(),
            [this](const SimulationTimeslice &state) 
            { 
                return 
                    ( clock_.now() >= state.start ) &&
                    ( clock_.now() < state.end );
            }
    );

    // advanced past end of scenario
    if (scenario_timeslice == scenario_.end())
    {
        done_ = true;
        return;
    }

    // update simulation state from scenario
    sensors_ = scenario_timeslice->sensors;

    printf("sensors_: ");
    for (unsigned lane = 0; lane < Lane::COUNT; ++lane)
    {
        printf("%d ", sensors_[lane]);
    }
    puts("");
}
