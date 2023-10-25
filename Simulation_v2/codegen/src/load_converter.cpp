#include "load_converter.h"

void load_converter::set_attributes(){

    current_in.set_rate(1);
    current_in.set_timestep(1, sc_core::SC_SEC);
    voltage_in.set_rate(1);
    voltage_in.set_timestep(1, sc_core::SC_SEC);
}

void load_converter::initialize() {}

void load_converter::processing(){
    double v_tmp;
    double i_tmp;
    double i_out;

    // Read input quantities
    v_tmp = voltage_in.read();
    i_tmp = current_in.read();
    //cattura voltage

    //Computed Current (Psensor * efficency / Vbus)
    if (v_tmp == 0) {
        i_out = 0.0;
    } else {
        i_out = ( (v_tmp*i_tmp)*efficency_factor ) / VREF_BUS;
    }

    current_out.write(i_out);

}

void load_converter::set_efficency(double k){

    efficency_factor = k;
}