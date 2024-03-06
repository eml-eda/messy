#include "${sensor_name}_functional.h"

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
                    // read operation
                    data_out.write(register_memory[address.read()]);
                    // reading
                    power_signal.write(${sensor_name}_read);
                    double start_time=sc_time_stamp().to_double();
                    core->go_ahead_for(start_time,${states["read"]["delay"]},SIM_RESOLUTION);
                    // idling
                    power_signal.write(${sensor_name}_idle);
                    go.write(true);
                } else {
                    // write operation
                    register_memory[address.read()] = data_in.read();
                    data_out.write(data_in.read());
                    // writing
                    power_signal.write(${sensor_name}_write);
                    double start_time=sc_time_stamp().to_double();
                    core->go_ahead_for(start_time,${states["write"]["delay"]},SIM_RESOLUTION);
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

