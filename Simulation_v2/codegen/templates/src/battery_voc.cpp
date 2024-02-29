#include "battery_voc.h"


void battery_voc::set_attributes()
{
    v_oc.set_timestep(1, SIM_RESOLUTION);
    r_s.set_timestep(1, SIM_RESOLUTION);
    soc.set_timestep(1, SIM_RESOLUTION);
    r_s.set_delay(1);
    soc.set_delay(1);
}

void battery_voc::initialize() {
    tmpsoc = {{starting_voc/100}}f;
}

void battery_voc::processing(){
    double tmpcurrent; // Battery current, if negative, the battery is charged 
    
    // Read input current
    tmpcurrent = i.read(); // Battery current, if negative, the battery is charged 

    /* 
    Compute actual state-of-charge solving the integral:
    SOC_t = SOC_{t-1} - \int^{t}_{-inf} i(\tau) / C d\tau
    */
    tmpsoc -= (((tmpcurrent + prev_i_batt) * 1) / (2 * (3600*1000) * c_nom)); // 3600 * 1000 Cnom, mAh to mAs cause [sim step] = [ms]
    prev_i_batt = tmpcurrent; // Update

    // Output the battery SOC
    if(tmpsoc >= 1) // Not let the SOC overflow
    {
        soc.write(1);
        tmpsoc = 1;
    }
    else    soc.write(tmpsoc);

    // SOC and battery Voc relationship
    // val(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
    double v_p1=-13.26,v_p2=27.4,v_p3=-17.99,v_p4=4.496,v_p5=3.45;
    v_oc.write((v_p1 * tmpsoc * tmpsoc * tmpsoc * tmpsoc) + (v_p2 * tmpsoc * tmpsoc * tmpsoc) + (v_p3 * tmpsoc * tmpsoc) + (v_p4 * tmpsoc) + v_p5); // Place interpolated funct here

    // SOC and battery internal resistance relationship
    // val(x) = p1*x^4 + p2*x^3 + p3*x^2 + p4*x + p5
    double r_p1=-0.1055,r_p2=0.1968,r_p3=-0.1119,r_p4=0.01607,r_p5=0.003117;
    r_s.write((r_p1 * tmpsoc * tmpsoc * tmpsoc * tmpsoc) + (r_p2 * tmpsoc * tmpsoc * tmpsoc) + (r_p3 * tmpsoc * tmpsoc) + (r_p4 * tmpsoc) + r_p5); // Place interpolated funct here

    // When the battery SOC decreases under 5% of starting voc, the simulation stops.	
    if(tmpsoc <= (${config["soc"]})
    {
        cout << "SOC is less than or equal to {{starting_voc-perc_voc}}/100%:" << " @" << sc_time_stamp() << " ; it is "<< tmpsoc << endl;
        sc_stop();
    }
}
