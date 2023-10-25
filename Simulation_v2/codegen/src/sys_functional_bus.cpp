#include "config.h"
#include <sys_functional_bus.h>

void functional_bus::processing_data(){
    int add_s;
    int data_s;
    int reg_s;
    bool flag_s;
    
    while (true){
        if (Ready_M.read() == true) {
            add_s = address_in_M.read();
            flag_s = flag_in_M.read();
            
            if (AIR_QUALITY_SENSOR_BASE_ADDRESS <= add_s && add_s <= (AIR_QUALITY_SENSOR_BASE_ADDRESS + AIR_QUALITY_SENSOR_REG_DIMENSION)) 
            {
                reg_s = add_s - AIR_QUALITY_SENSOR_BASE_ADDRESS;
                data_s = data_in_M.read();
                //cout << data_s << endl;

                address_out_S[1].write(reg_s);
                data_out_S[1].write(data_s);
                flag_out_S[1].write(flag_s);
                selected_slave = 1;
                Set_Slave(selected_slave, true);
            }

            if (TEMPERATURE_SENSOR_BASE_ADDRESS <= add_s && add_s <= (TEMPERATURE_SENSOR_BASE_ADDRESS + TEMPERATURE_SENSOR_REG_DIMENSION)) 
            {
                reg_s = add_s - TEMPERATURE_SENSOR_BASE_ADDRESS;
                data_s = data_in_M.read();
                //cout << data_s << endl;

                address_out_S[2].write(reg_s);
                data_out_S[2].write(data_s);
                flag_out_S[2].write(flag_s);
                selected_slave = 2;
                Set_Slave(selected_slave, true);
            }

            if (METHANE_SENSOR_BASE_ADDRESS <= add_s && add_s <= (METHANE_SENSOR_BASE_ADDRESS + METHANE_SENSOR_REG_DIMENSION)) 
            {
                reg_s = add_s - METHANE_SENSOR_BASE_ADDRESS;
                data_s = data_in_M.read();
                //cout << data_s << endl;

                address_out_S[3].write(reg_s);
                data_out_S[3].write(data_s);
                flag_out_S[3].write(flag_s);
                selected_slave = 3;
                Set_Slave(selected_slave, true);
            }

            }
        wait();

        response();

        while (Go_S[selected_slave].read() != false && Ready_M.read() != false) {
            wait();
        }
        
        Set_Go(false);
 
    }
}    

void functional_bus::response(){
    if (Go_S[selected_slave].read() == true) {
        data_out_M.write(data_in_S[selected_slave].read());
        Set_Go(true);
        Set_Slave(selected_slave, false);
    }
}

void functional_bus::Set_Go(bool flag){
    Go_M.write(flag);
}

void functional_bus::Set_Slave(int address, bool flag){
    Ready_S[address].write(flag);
}