#include "battery_voc.h"

void battery_voc::set_attributes() {
    v_oc.set_timestep(1, sc_core::SC_SEC);
    r_s.set_timestep(1, sc_core::SC_SEC);
    soc.set_timestep(1, sc_core::SC_SEC);
    r_s.set_delay(1);
    soc.set_delay(1);
}

void battery_voc::initialize() {}

void battery_voc::processing() {
    double tmpcurrent; // Battery current, if negative, the battery is charged

    // Read input current
    tmpcurrent = i.read(); // Battery current, if negative, the battery is charged

    /*
    Compute actual state-of-charge solving the integral:
    SOC_t = SOC_{t-1} - \int^{t}_{-inf} i(\tau) / C d\tau
    */
    int c_nom = 3200;
    tmpsoc -= (((tmpcurrent + prev_i_batt) * 1) / (2 * 3600 * c_nom)); // 3600 * Cnom, mAh to mAs cause [sim step] = [s]
    prev_i_batt = tmpcurrent;                                          // Update

    // Each instant the battery self-discharge a bit
    tmpsoc = (1.0 - 0.0) * tmpsoc;

    // Output the battery SOC
    if (tmpsoc >= 1) // Not let the SOC overflow
    {
        soc.write(1);
        tmpsoc = 1;
    } else {
        soc.write(tmpsoc);
    }

    // SOC and battery Voc relationship
    v_oc.write((2.419 * tmpsoc * tmpsoc * tmpsoc) + (-4.232 * tmpsoc * tmpsoc) + (3.154 * tmpsoc) + 2.622); // Place interpolated funct here

    // SOC and battery internal resistance relationship
    r_s.write((3.371e-4 * tmpsoc * tmpsoc * tmpsoc) + (-6.423e-4 * tmpsoc * tmpsoc) + (3.372e-4 * tmpsoc) + 2.054e-5); // Place interpolated funct here

    // When the battery SOC decreases under 1%, the simulation stops.
    if (tmpsoc <= 0.01) {
        cout << "SOC is less than or equal to 1%:"
             << " @" << sc_time_stamp() << endl;
        sc_stop();
    }
}
