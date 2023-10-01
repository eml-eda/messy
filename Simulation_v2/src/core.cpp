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

    while(1){

        double time = double(sc_time_stamp().to_double());

        //std::cout << time << std::endl;
        
        int64_t next_timestamp = gvsoc->step_until(time);

        //std::cout << next_timestamp << std::endl;

        if(next_timestamp == -1){
            break;
        } else {
            wait(next_timestamp - time, sc_core::SC_PS);
        }
    }

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
 
    if (req->type == gv::Io_request_read)
    {
        printf("Received request (is_read: %d, addr: 0x%lx, size: 0x%lx, data: %d)\n", req->type == gv::Io_request_read, req->addr, req->size , *(req->data));
        D_Out.write(1);
        F_Out.write(true);
        Ready.write(true);
        switch (req->addr)
        {
            case 0x00:
            A_Out.write(101);
            break;

            case 0x04:
            A_Out.write(201);
            break;

            case 0x08:
            A_Out.write(301);
            break;

            case 0x0C:
            A_Out.write(401);
            break;

            case 0x10:
            A_Out.write(501);
            break;
        }

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
        switch (req->addr)
        {
            case 0x00:
            A_Out.write(101);
            break;

            case 0x04:
            A_Out.write(201);
            break;

            case 0x08:
            A_Out.write(301);
            break;

            case 0x0C:
            A_Out.write(401);
            break;

            case 0x10:
            A_Out.write(501);
            break;
        }

        wait();
        Ready.write(false);
        wait();
        
    }

    *((uint32_t*)req->data) = Data_in.read();
    
    printf("%d , %lx , %lx \n", *(req->data), req->addr, req->size);
    
    this->axi->reply(req);
    
}

void Core::grant(gv::Io_request *req)
{
}

void Core::reply(gv::Io_request *req)
{
}