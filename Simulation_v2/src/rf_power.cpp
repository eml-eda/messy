#include "rf_power.h"

void rf_power::set_attributes() {
    func_signal.set_rate(1);
    func_signal.set_timestep(1, sc_core::SC_SEC);
    // power_to_therm.set_timestep(0.5, sc_core::SC_SEC);
    // power_to_therm.set_rate(1);
    // power_to_therm.set_delay(1);
}

void rf_power::initialize() {}

void rf_power::processing() {
    if (func_signal.read() == 1) {
        // std::cout << "Radio Frequency in ON state READ MODE" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(RF_ON_I_ON);
        // power_to_therm.write(5.0);
        return;
    }

    if (func_signal.read() == 2) {
        // std::cout << "Radio Frequency in ON state WRITE MODE" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(10);
        // power_to_therm.write(5.0);
        return;
    }

    if (func_signal.read() == 3) {
        // std::cout << "Radio Frequency in IDLE state" << std::endl;
        voltage_state.write(VREF_BUS);
        current_state.write(RF_I_IDLE);
        // power_to_therm.write(0.0);
        return;
    }

    if (func_signal.read() == 0) {
        // std::cout << "Radio Frequency in OFF state" << std::endl;
        voltage_state.write(0.0);
        current_state.write(0.0);
        // power_to_therm.write(0.0);
        return;
    }

    std::cout << "Radio Frequency in an Unknown state!" << std::endl;
    // power_to_therm.write(0.0);
}