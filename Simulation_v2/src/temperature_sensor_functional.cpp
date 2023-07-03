#include "temperature_sensor_functional.h"

void temperature_sensor_functional::sensor_logic(){
    Register[1] = 1;
    Register[3] = 15;
    Register[6] = 75;
while (true)
{
    if( enable.read() == true ){
        if( flag_wr.read() == true ){
            //Read Operations
            //int rnd = 1 + (rand() % 100);
            data_out.write(Register[address.read()]);
            //data_out.write(rnd);
            power_signal.write(1);
            //cout << "Term Read " << Register[address.read()] << endl;
            
        } else {
            //Write Operations
            Register[address.read()] = data_in.read();
            data_out.write(data_in.read());
            power_signal.write(2);
            //cout << "Term Write " << Register[address.read()] << endl;
        }

    } else {
        //std::cout << "The sensor is not enable!" << endl;
        power_signal.write(0);
    }
    wait();
}   
}
