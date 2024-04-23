#include <adapters/adapter_gvsoc.hpp>

AdapterGvsoc::AdapterGvsoc()
{
    #ifdef LOG_REQUESTS
    request_addresses.open("./request_adresses.log");
    request_sizes.open("./request_sizes.log");
    #endif
}

void AdapterGvsoc::has_ended(){
    this->finished=true;
    return;
}

void AdapterGvsoc::has_stopped(){
    return;
}

void AdapterGvsoc::was_updated(){
    return;
}

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

MessyRequest* AdapterGvsoc::get_messy_request_from_gvsoc(gv::Io_request* req){
    #ifdef LOG_REQUESTS
    request_addresses << req->addr << std::endl;
    request_sizes << req->size << std::endl;
    #endif
    return (MessyRequest*)new MessyRequest((long long)((req->addr-0x20000000)/4),(unsigned int*)req->data,(bool)req->type==gv::Io_request_read,(unsigned int*)req->handle);
}

int64_t AdapterGvsoc::exec_events_at(int64_t timestamp){
    return gvsoc->step_until(timestamp);
}

double AdapterGvsoc::get_power_at(int64_t timestamp){
    double x=0,y=0;
    return this->gvsoc->get_instant_power(x,y);
}

void AdapterGvsoc::close(){
    if(!closed){
        int retval = gvsoc->join();
        //std::cout << gvsoc->stop() << std::endl;
        gvsoc->stop();
        gvsoc->close();
        closed=1;
    }
}

void AdapterGvsoc::access(gv::Io_request* req){
    add_request(this->get_messy_request_from_gvsoc(req));
}

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