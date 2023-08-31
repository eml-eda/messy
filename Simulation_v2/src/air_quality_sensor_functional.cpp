#include "air_quality_sensor_functional.h"

void air_quality_sensor_functional::sensor_logic(){

    char config_path[18] = "gvsoc_config.json";

    gv::GvsocConf conf = { .config_path=config_path, .api_mode=gv::Api_mode::Api_mode_sync };
    gv::Gvsoc *gvsoc = gv::gvsoc_new(&conf);
    gvsoc->open();

    // Get a connection to the main soc AXI. This will allow us to inject accesses
    // and could also be used to received accesses from simulated test
    // to a certain mapping corresponding to the external devices.
    this->axi = gvsoc->io_bind(this, "/chip/soc/axi_proxy", "");
    if (this->axi == NULL)
    {
        fprintf(stderr, "Couldn't find AXI proxy\n");
        return;
    }

    //Register[1] = 1;
    Register[3] = 20;
    Register[6] = 15;
    while (true)
    {
        if( enable.read() == true ){
            if(ready.read() == true){

                if( flag_wr.read() == true ){
                    //Read Operations   
                    //int rnd = 1 + (rand() % 100);
                    //data_out.write(rnd);
                    data_out.write(Register[address.read()]);
                    power_signal.write(1);
                    wait(AIR_QUALITY_SENSOR_T_ON,sc_core::SC_SEC);
                    power_signal.write(3);
                    go.write(true);
                } else {
                    //Write Operations
                    Register[address.read()] = data_in.read();
                    data_out.write(data_in.read());
                    
                    //Trying to update value in GvSoC Memory
                    gv::Io_request req;
                    req.data = (uint8_t *)&data_in.read();
                    req.size = 4;
                    req.type = gv::Io_request_write;
                    req.addr = 0x80000000;

                    power_signal.write(2);
                    wait(AIR_QUALITY_SENSOR_T_ON,sc_core::SC_SEC);
                    power_signal.write(3);
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

void air_quality_sensor_functional::access(gv::Io_request *req)
{
}

void air_quality_sensor_functional::grant(gv::Io_request *req)
{
}

void air_quality_sensor_functional::reply(gv::Io_request *req)
{
}