#include "converter_pv.h"

void converter_pv::set_attributes() {
    i_out.set_timestep(1, sc_core::SC_SEC);
    // i_out.set_delay(1);
}

void converter_pv::initialize() {}

void converter_pv::processing() {
    double ideal_power;
    double eta; // Efficiency
    double i_tmp;
    double v_tmp;

    // Read input quantities
    i_tmp = i_in.read();
    v_tmp = v_in.read();

    // Ideal power
    ideal_power = i_tmp * v_tmp;

    // Get efficiency
    eta = lut_eta.get_val(v_tmp) / 100;

    i_out.write(ideal_power * eta / VREF_BUS);
}
