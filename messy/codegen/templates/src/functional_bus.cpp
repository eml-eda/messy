#include "config.h"
#include <functional_bus.h>

void functional_bus::processing_data(){
    int add_s;
    int data_s;
    int reg_s;
    bool flag_s;
    
    while (true){
        if (request_ready.read() == true) {
            add_s = request_address.read();
            flag_s = flag_in_M.read();
            
            % for idx,(sensor_name,sensor) in enumerate(peripherals["sensors"].items()):
            if (${sensor_name.upper()}}_BASE_ADDRESS <= add_s && add_s <= (${sensor_name.upper()}_BASE_ADDRESS + ${sensor["register_memory"]})) 
            {
                reg_s = add_s - ${sensor_name.upper()}_BASE_ADDRESS;
                data_s = request_data.read();
                //cout << data_s << endl;

                address_out_sensor[${idx}].write(reg_s);
                data_out_sensor[${idx}].write(data_s);
                flag_out_sensor[${idx}].write(flag_s);
                selected_slave = ${idx};
                Set_Slave(selected_slave, true);
            }
            % endfor
        }
        wait();

        response();

        while (Go_S[selected_slave].read() != false && request_ready.read() != false) {
            wait();
        }
        
        Set_Go(false);
 
    }
}    

void functional_bus::response(){
    if (go_sensor[selected_slave].read() == true) {
        request_value.write(data_in_sensor[selected_slave].read());
        Set_Go(true);
        Set_Slave(selected_slave, false);
    }
}

void functional_bus::Set_Go(bool flag){
    request_go.write(flag);
}

void functional_bus::Set_Slave(int address, bool flag){
    ready_sensor[address].write(flag);
}