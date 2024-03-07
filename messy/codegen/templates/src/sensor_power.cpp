#include <sensor_${sensor_name}_power.hpp>

void Sensor_${sensor_name}_power::set_attributes()
{
    func_signal.set_rate(1);
    func_signal.set_timestep(1, SIM_RESOLUTION);
}

void Sensor_${sensor_name}_power::initialize() {}

void Sensor_${sensor_name}_power::processing()
{
    % for state_name,state in states.items():
    if(func_signal.read() == ${sensor_name}_${state_name} ){
        voltage_state.write(${sensor_name}_VREF);
        current_state.write(${state["current"]});
        return;
    }
    % endfor
    if(func_signal.read() == 0){
        voltage_state.write(0.0);
        current_state.write(0.0);
        return;
    }
}