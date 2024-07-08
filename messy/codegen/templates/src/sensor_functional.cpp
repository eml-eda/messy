#include <sensor_${sensor_name}_functional.hpp>

void Sensor_${sensor_name}_functional::sensor_logic(){
    % if memory_init:
    % for idx,val in enumerate(memory_init):
    register_memory[idx]=val;
    % endfor
    % endif
    while (true)
    {
        if( enable.read() == true ){
            if(ready.read() == true){
                if( flag_wr.read() == true ){
                    unsigned int add=address.read();
                    // read operation
                    data_out.write(register_memory+add);
                    // reading
                    power_signal.write(${sensor_name}_read);
                    double start_time=sc_time_stamp().to_double();
                    core->request_delay(start_time,${states["read"]["delay"]},SIM_RESOLUTION);
                    // idling
                    power_signal.write(${sensor_name}_idle);
                    go.write(true);
                } else {
                    unsigned int add=address.read();
                    // write operation
                    unsigned int req_size_val=req_size.read();
                    uint8_t* req_core_val_addr=data_in.read();
                    for(unsigned int i=0;i<req_size_val;i++) register_memory[i+add] = req_core_val_addr[i];
                    data_out.write(register_memory+add);
                    // writing
                    power_signal.write(${sensor_name}_write);
                    double start_time=sc_time_stamp().to_double();
                    core->request_delay(start_time,${states["write"]["delay"]},SIM_RESOLUTION);
                    // idling
                    power_signal.write(${sensor_name}_idle);
                    go.write(true);
                }
            } else {
                go.write(false);
            }

        } else {
            power_signal.write(0);
        }
        wait();
    }   
}

