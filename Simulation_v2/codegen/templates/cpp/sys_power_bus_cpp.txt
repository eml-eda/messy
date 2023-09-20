#include "sys_power_bus.h"

void power_bus::set_attributes(){}

void power_bus::initialize(){}

void power_bus::processing(){
    double tmp_pv;
    double tmp_i;
    total_current = 0;

    for (int i = 0; i < NP; i++){

        tmp_i = current_in_S[i].read();
        total_current = total_current + tmp_i;
    }
    
    tmp_i = CPU_I.read();
    tmp_pv = PV_I.read();

    total_current = total_current + tmp_i - tmp_pv;

    //std::cout << total_current << " A " << sc_time_stamp() << endl;
    battery_out_current.write(total_current);
}