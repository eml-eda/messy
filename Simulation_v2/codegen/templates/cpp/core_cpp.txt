#include "core.h"

void Core::run()
{
    char config_path[18] = "gvsoc_config.json";

    gv::GvsocConf conf = { .config_path=config_path, .api_mode=gv::Api_mode::Api_mode_sync };
    gvsoc = gv::gvsoc_new(&conf);
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

    gvsoc->start();
    
    // Run for 5ms to let the chip boot as it is not accessible before it is powered-up
    //gvsoc->step(5000000000);
    // Wait for simulation termination and exit code returned by simulated test

    // Run for 10ms to let the chip boot as it is not accessible before it is powered-up
    //    gvsoc->step(10000000000);

    while(1){

        double old_time = double(sc_time_stamp().to_double());

        int64_t next_timestamp = gvsoc->step_until(old_time);

        double time = double(sc_time_stamp().to_double());

        if (next_timestamp == -1){
            break;
        } else {
            if (old_time != time){
                next_timestamp = gvsoc->step_until(time);
                printf("%d, %lx, %lx \n", *(req_global->data), req_global->addr, req_global->size);
                this->axi->reply(req_global);
            } else {
                //Update CPU Consumption
                wait(next_timestamp - time, sc_core::SC_PS);
                //Restore CPU State
            }
        }
    }

    // Wait for simulation termination and exit code returned by simulated test
    int retval = gvsoc->join();
    std::cout << sc_time_stamp().to_double() << std::endl;
    std::cout << gvsoc->stop() << std::endl;
    //gvsoc->stop();
    gvsoc->close();
    sc_stop();
    return;
}


void Core::access(gv::Io_request *req)
{
    //Here connect the GvSoc signals with SystemC simulator signals   
    std::cout << sc_time_stamp().to_double() << std::endl;
    int tmpaddr;
    if (req->type == gv::Io_request_read)
    {
        printf("Received request (is_read: %d, addr: 0x%lx, size: 0x%lx, data: %d)\n", req->type == gv::Io_request_write, req->addr, req->size , *(req->data));
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        tmpaddr = req->addr;
        //std::cout << tmpaddr << std::endl;

        A_Out.write(tmpaddr);

        wait();
        Ready.write(false);
        wait();

    }
    else
    {
        printf("Received request (is_write: %d, addr: 0x%lx, size: 0x%lx, data: %d)\n", req->type == gv::Io_request_write, req->addr, req->size , *(req->data));
        D_Out.write(*(req->data));
        F_Out.write(false);
        Ready.write(true);
        tmpaddr = req->addr;
        //std::cout << tmpaddr << std::endl;

        A_Out.write(tmpaddr);

        wait();
        Ready.write(false);
        wait();
        
    }
    *((uint32_t*)req->data) = Data_in.read();

    req_global = req;

    printf("%d, %lx, %lx \n", *(req_global->data), req_global->addr, req_global->size);
    
    std::cout << sc_time_stamp().to_double() << std::endl;
    //this->axi->reply(req);
}

void Core::grant(gv::Io_request *req)
{
}

void Core::reply(gv::Io_request *req)
{
}