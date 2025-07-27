#include <adapters/adapter_gvsoc_gap9.hpp>

/**
 * @brief Constructor for AdapterGvsocGap9.
 *
 * Initializes the AdapterGvsocGap9 object and opens log files if logging is enabled.
 */
AdapterGvsocGap9::AdapterGvsocGap9()
{
    #ifdef LOG_REQUESTS
    request_addresses.open("./request_adresses.log");
    request_sizes.open("./request_sizes.log");
    #endif
}

/**
 * @brief Starts up the GVSOC GAP9 adapter.
 *
 * This function initializes and starts up the GVSOC GAP9 adapter by configuring the GVSOC
 * settings, binding to the AXI interface, and starting the GVSOC simulation. It also registers
 * this core as an executor notifier.
 */
void AdapterGvsocGap9::startup(){
    string gvsoc_config_path="${config['path']}";
    gv::GvsocConf conf;
    conf.config_path=gvsoc_config_path;
    conf.api_mode=gv::Api_mode::Api_mode_sync;
    conf.proxy_socket=-1;
    gvsoc = (Gvsoc_launcher*) gv::gvsoc_new(&conf);
    gvsoc->open();
    // register this core as an executor notifier
    gvsoc->register_exec_notifier(this);

    // Get a connection to the main soc AXI. This will allow us to inject accesses
    // and could also be used to received accesses from simulated test
    // to a certain mapping corresponding to the external devices.
    this->axi = gvsoc->io_bind(this, "${config['axi_path']}", "");
    if (this->axi == NULL)
    {
        fprintf(stderr, "Couldn't find AXI proxy\n");
        return;
    }

    gvsoc->start();
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
MessyRequest* AdapterGvsocGap9::get_messy_request_from_gvsoc(gv::Io_request* req){
    #ifdef LOG_REQUESTS
    request_addresses << req->addr << std::endl;
    request_sizes << req->size << std::endl;
    #endif
    return (MessyRequest*)new MessyRequest((long long)req->addr,(unsigned int*)req->data,(bool)req->type==gv::Io_request_read,(unsigned int*)req->handle,req->size);
}

/**
 * @brief Executes events at the specified timestamp.
 *
 * This function advances the GVSOC GAP9 simulation to the specified timestamp.
 *
 * @param timestamp The timestamp at which to execute events.
 * @return The next timestamp at which events should be executed.
 */
int64_t AdapterGvsocGap9::exec_events_at(int64_t timestamp){
    int64_t next_event_time=gvsoc->step_until(timestamp);
    finished=this->gvsoc->top_get()->time_engine->finished_get();
    return next_event_time;
}

/**
 * @brief Retrieves the power consumption at the specified timestamp.
 *
 * This function retrieves the instantaneous power consumption from the GVSOC GAP9 simulation
 * at the specified timestamp.
 *
 * @param timestamp The timestamp at which to retrieve power consumption.
 * @return The power consumption at the specified timestamp.
 */
double AdapterGvsocGap9::get_power_at(int64_t timestamp){
    double x=0,y=0;
    double inst_power=this->gvsoc->get_instant_power(x,y);
    return inst_power;
}

/**
 * @brief Closes the GVSOC GAP9 adapter.
 *
 * This function closes the GVSOC GAP9 adapter, stops the GVSOC simulation, and performs any
 * necessary cleanup.
 */
void AdapterGvsocGap9::close(){
    if(!closed){
        int retval = gvsoc->join();
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
void AdapterGvsocGap9::access(gv::Io_request* req){
    add_request(this->get_messy_request_from_gvsoc(req));
}

/**
 * @brief Custom reply handler for MessyRequest.
 *
 * This function handles the reply for a MessyRequest. It retrieves the corresponding
 * vp::io_req object, sends the response through the response port, and deletes the request.
 *
 * @param req Pointer to the MessyRequest to be handled.
 */
void AdapterGvsocGap9::custom_reply(MessyRequest* req){
    vp::io_req *vp_req = (vp::io_req *)req->handle_c;
    vp_req->get_resp_port()->resp(vp_req);
    delete req;
}

/**
 * @brief Handles granted requests from GVSOC.
 *
 * This function is called when one of our access requests gets granted. Currently, it does nothing.
 *
 * @param req Pointer to the Io_request that was granted.
 */
void AdapterGvsocGap9::grant(gv::Io_request *req)
{

}

/**
 * @brief Handles replies to requests from GVSOC.
 *
 * This function is called when one of our access requests gets a reply. Currently, it does nothing.
 *
 * @param req Pointer to the Io_request that received a reply.
 */
void AdapterGvsocGap9::reply(gv::Io_request *req)
{

}

/**
 * @brief Notifies that the simulation has stopped.
 *
 * This function is called by GVSOC to notify that the simulation has stopped. If there are no
 * retained counts, it releases the GVSOC launcher.
 *
 * @param time The time at which the simulation stopped.
 */
void AdapterGvsocGap9::notify_stop(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->release();
}

/**
 * @brief Notifies that the simulation is running.
 *
 * This function is called by GVSOC to notify that the simulation is running. If there are no
 * retained counts, it retains the GVSOC launcher.
 *
 * @param time The time at which the simulation started running.
 */
void AdapterGvsocGap9::notify_run(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->retain();
}
