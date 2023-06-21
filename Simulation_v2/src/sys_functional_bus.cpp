#include "config.h"
#include <sys_functional_bus.h>

void functional_bus::processing_data(){
    int add_s;
    int data_s;
    int reg_s;
    bool flag_s;
    
    while (true){
        add_s = address_in_M.read();
        flag_s = flag_in_M.read();

        if (Air_Base_Address <= add_s && add_s <= (Air_Base_Address + Air_Reg_Dimension)) 
        {
            reg_s = add_s - Air_Base_Address;
            data_s = data_in_M.read();
            //cout << data_s << endl;

            address_out_S[1].write(reg_s);
            data_out_S[1].write(data_s);
            flag_out_S[1].write(flag_s);
            selected_slave = 1;
        }

        if (Temp_Base_Address <= add_s && add_s <= (Temp_Base_Address + Temp_Reg_Dimension))
        {
            reg_s = add_s - Temp_Base_Address;
            data_s = data_in_M.read();
            //cout << data_s << endl;

            address_out_S[2].write(reg_s);
            data_out_S[2].write(data_s);
            flag_out_S[2].write(flag_s);
            selected_slave = 2; 
        
        }

        wait();
    }    
}

void functional_bus::response(){
    data_out_M.write(data_in_S[selected_slave].read());
}