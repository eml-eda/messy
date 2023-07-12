#include "sys_power_bus.h"

void power_bus::set_attributes(){}

void power_bus::initialize(){}

void power_bus::processing(){
    double tmp_v;
    double tmp_i;
    total_voltage = 0;
    total_current = 0;

    for (int i = 0; i < NP; i++){
        tmp_v = voltage_in_S[i].read();
        tmp_i = current_in_S[i].read();
        total_voltage = total_voltage + tmp_v;
        total_current = total_current + tmp_i;
    }

    std::cout << total_voltage << " V " << sc_time_stamp() << endl;
    std::cout << total_current << " A " << sc_time_stamp() << endl;
    voltage_out_sum.write(total_voltage);
    battery_out_current.write(total_current);
}