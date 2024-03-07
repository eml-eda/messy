#include <power_bus.hpp>

void Power_bus::set_attributes(){}

void Power_bus::initialize(){}

void Power_bus::processing(){
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