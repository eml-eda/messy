#include "battery_converter.h"

void battery_converter::set_attributes() {
    current_out.set_timestep(1, sc_core::SC_SEC);
    current_out.set_delay(1);
}

void battery_converter::initialize() {}

void battery_converter::processing() {
    double eta; // Efficiency
    double i_bus_tmp;
    double v_batt_tmp;

    // Read input quantities
    i_bus_tmp = current_in.read();
    v_batt_tmp = voltage_in.read();

    // Avoid div by zero
    if (v_batt_tmp == 0) {
        v_batt_tmp = 3.6;
    }

    // Get efficiency
    eta = lut_eta.get_val(fabs(i_bus_tmp)) / 100;

    current_out.write((eta * (i_bus_tmp * VREF_BUS)) / v_batt_tmp);
}
