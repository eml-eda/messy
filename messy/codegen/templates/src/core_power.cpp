#include <core_power.hpp>

void Core_power::set_attributes()
{
    func_signal.set_rate(1);
    func_signal.set_timestep(1, SIM_RESOLUTION);
}

void Core_power::initialize() {}

void Core_power::processing()
{
    if(!core->simulation_iters){
        voltage_state.write(0.0);
        current_state.write(0.0);
        return;
    }
    voltage_state.write(V_CORE);
    current_state.write(((core->tot_power/core->simulation_iters)*1000)/V_CORE);
    core->simulation_iters=0;
    core->tot_power=0;
}
