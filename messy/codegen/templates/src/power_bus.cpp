#include "power_bus.h"

void power_bus::set_attributes(){}

void power_bus::initialize(){}

void power_bus::processing(){
    double tmp_i;
    total_current = 0;

    for (int i = 0; i < NUM_SENSORS; i++){

        tmp_i = current_sensors[i].read();
        total_current = total_current + tmp_i;
    }
    
    tmp_i = core_current.read();

    total_current = total_current + tmp_i;
    
    battery_out_current.write(total_current);
}