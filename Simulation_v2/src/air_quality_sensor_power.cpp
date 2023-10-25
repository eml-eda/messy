#include "air_quality_sensor_power.h"

void air_quality_sensor_power::set_attributes() {
    func_signal.set_rate(1);
    func_signal.set_timestep(1, sc_core::SC_SEC);
    // power_to_therm.set_timestep(0.5, sc_core::SC_SEC);
    // power_to_therm.set_rate(1);
    // power_to_therm.set_delay(1);
}

void air_quality_sensor_power::initialize() {}

void air_quality_sensor_power::processing() {
    if (func_signal.read() == 1) {
        // std::cout << "Air quality in ON state READ MODE" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(AIR_QUALITY_SENSOR_I_ON);
        // power_to_therm.write(5.0);
        return;
    }

    if (func_signal.read() == 2) {
        // std::cout << "Air quality in ON state WRITE MODE" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(10);
        // power_to_therm.write(5.0);
        return;
    }

    if (func_signal.read() == 3) {
        // std::cout << "Air quality in IDLE state" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(AIR_QUALITY_SENSOR_I_IDLE);
        // power_to_therm.write(0.0);
        return;
    }

    if (func_signal.read() == 0) {
        // std::cout << "Air quality in OFF state" << std::endl;
        voltage_state.write(0.0);
        current_state.write(0.0);
        // power_to_therm.write(0.0);
        return;
    }

    std::cout << "Air quality in an Unknown state!" << std::endl;
    // power_to_therm.write(0.0);
}