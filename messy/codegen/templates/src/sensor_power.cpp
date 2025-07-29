#include <sensor_${sensor_name}_power.hpp>

void Sensor_${sensor_name}_power::set_attributes()
{
    i_power_state.set_rate(1);
    i_power_state.set_timestep(1, SIM_RESOLUTION);
}

void Sensor_${sensor_name}_power::initialize() {}

/**
 * @warning This function should not be called directly. It is called by the SystemC-AMS kernel.
 * 
 * @brief Process the functional signal and output voltage and current state.
 * 
 * This method reads the current functional state of the sensor and updates
 * the voltage and current values accordingly. The voltage and current values
 * are based on predefined states and associated current consumption. 
 * 
 * The available states are:
 % for state_name,state in states.items():
 *   ${sensor_name}_${state_name} : Voltage = ${sensor_name}_VREF, Current = ${state["current"]}
 % endfor
 */
void Sensor_${sensor_name}_power::processing()
{
    % for state_name,state in states.items():
    if(i_power_state.read() == ${sensor_name}_${state_name} ){
        o_voltage_a.write(${sensor_name}_VREF);
        o_current_a.write(${state["current"]});
        return;
    }
    % endfor
    if(i_power_state.read() == 0){
        o_voltage_a.write(0.0);
        o_current_a.write(0.0);
        return;
    }
}