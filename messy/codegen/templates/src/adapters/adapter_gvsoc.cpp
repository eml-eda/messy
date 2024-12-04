#include <adapters/adapter_gvsoc.hpp>

/**
 * @brief Constructor for AdapterGvsoc.
 * 
 * Initializes the AdapterGvsoc object and opens log files if logging is enabled.
 */
AdapterGvsoc::AdapterGvsoc()
{
    #ifdef LOG_REQUESTS
    request_addresses.open("./request_adresses.log");
    request_sizes.open("./request_sizes.log");
    #endif
}

/**
 * @brief Called by GVSOC to notify the simulation has ended. It sets the finished flag to true.
 *
 * This means the simulated software is over and probably exited and GVSOC cannot further
 * simulate it.
 */
void AdapterGvsoc::has_ended(){
    this->finished=true;
    return;
}

/**
 * @brief Called by GVSOC to notify the simulation has stopped.
 *
 * This means an event occurred which stopped the simulation. Simulation can still be
 * resumed.
 */
void AdapterGvsoc::has_stopped(){
    return;
}

/**
 * @brief Called by GVSOC to notify the simulation engine was updated.
 *
 * This means a new event was posted to the engine and modified the timestamp of the next
 * event to be executed.
 */
void AdapterGvsoc::was_updated(){
    return;
}

/**
 * @brief Starts up the GVSOC adapter.
 * 
 * This function initializes and starts up the GVSOC adapter by configuring the GVSOC
 * settings, binding to the AXI interface, and starting the GVSOC simulation. Check this for additional details: https://gvsoc.readthedocs.io/en/latest/tutorials.html#how-to-control-gvsoc-from-an-external-simulator
 */
void AdapterGvsoc::startup(){
    string gvsoc_config_path="${config['path']}";
    gv::GvsocConf conf;
    conf.config_path=gvsoc_config_path;
    conf.api_mode=gv::Api_mode::Api_mode_sync;
    conf.proxy_socket=-1;
    gvsoc = (gv::GvsocLauncher*) gv::gvsoc_new(&conf);
    gvsoc->open();

    // Get a connection to the main soc AXI. This will allow us to inject accesses
    // and could also be used to received accesses from simulated test
    // to a certain mapping corresponding to the external devices.
    this->axi = gvsoc->io_bind(this, "${config['axi_path']}", "");
    if (this->axi == NULL)
    {
        fprintf(stderr, "Couldn't find AXI proxy\n");
        return;
    }
    gvsoc->vcd_disable();
    gvsoc->start();
    gvsoc->bind((gv::Gvsoc_user*)this);
}

/**
 * @brief Converts a GVSOC Io_request to a MessyRequest.
 * 
 * This function logs the request details if logging is enabled and converts the
 * GVSOC Io_request to a MessyRequest.
 * 
 * @param req Pointer to the Io_request from GVSOC.
 * @return Pointer to the corresponding MessyRequest.
 */
MessyRequest* AdapterGvsoc::get_messy_request_from_gvsoc(gv::Io_request* req){
    #ifdef LOG_REQUESTS
    request_addresses << req->addr << std::endl;
    request_sizes << req->size << std::endl;
    #endif
    return (MessyRequest*)new MessyRequest((long long)req->addr-0x20000000,(unsigned int*)req->data,(bool)req->type==gv::Io_request_read,(unsigned int*)req->handle,req->size);
}


/**
 * @brief Executes events at the specified timestamp.
 * 
 * This function advances the GVSOC simulation to the specified timestamp.
 * 
 * @param timestamp The timestamp at which to execute events.
 * @return The next timestamp at which events should be executed.
 */
int64_t AdapterGvsoc::exec_events_at(int64_t timestamp){
    return gvsoc->step_until(timestamp);
}

/**
 * @brief Retrieves the power consumption at the specified timestamp.
 * 
 * This function retrieves the instantaneous power consumption from the GVSOC simulation
 * at the specified timestamp.
 * 
 * @param timestamp The timestamp at which to retrieve power consumption.
 * @return The power consumption at the specified timestamp.
 */
double AdapterGvsoc::get_power_at(int64_t timestamp){
    double x=0,y=0;
    return this->gvsoc->get_instant_power(x,y);
}

/**
 * @brief Closes the GVSOC adapter.
 * 
 * This function closes the GVSOC adapter, stops the GVSOC simulation, and performs any
 * necessary cleanup.
 */
void AdapterGvsoc::close(){
    if(!closed){
        int retval = gvsoc->join();
        //std::cout << gvsoc->stop() << std::endl;
        gvsoc->stop();
        gvsoc->close();
        closed=1;
    }
}

/**
 * @brief Handles access requests from GVSOC.
 * 
 * This function is called when an access request is received from GVSOC. It converts
 * the GVSOC Io_request to a MessyRequest and adds it to the request queue.
 * 
 * @param req Pointer to the Io_request from GVSOC.
 */
void AdapterGvsoc::access(gv::Io_request* req){
    add_request(this->get_messy_request_from_gvsoc(req));
}

/**
 * @brief Custom reply handler for MessyRequest.
 * 
 * This function handles the reply for a MessyRequest. It retrieves the corresponding
 * vp::IoReq object, sends the response through the response port, and deletes the request.
 * 
 * @param req Pointer to the MessyRequest to be handled.
 */
void AdapterGvsoc::custom_reply(MessyRequest* req){
    vp::IoReq *vp_req = (vp::IoReq *)req->handle_c;
    vp_req->get_resp_port()->resp(vp_req);
    delete req;
}

void AdapterGvsoc::grant(gv::Io_request *req)
{

}

void AdapterGvsoc::reply(gv::Io_request *req)
{

}