#include <adapters/adapter_gvsoc.hpp>

AdapterGvsoc::AdapterGvsoc()
{
}
void AdapterGvsoc::startup(){
    char config_path[${len(config["path"])+1}] = "${config['path']}";

    gv::GvsocConf conf = { .config_path=config_path, .api_mode=gv::Api_mode::Api_mode_sync };
    gvsoc = (Gvsoc_launcher*) gv::gvsoc_new(&conf);
    gvsoc->open();
    // register this core as an executor notifier
    gvsoc->register_exec_notifier(this);

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
}

MessyRequest* AdapterGvsoc::get_messy_request_from_gvsoc(gv::Io_request* req){
    return (MessyRequest*)new MessyRequest(0,0,true,0x0);
}

int64_t AdapterGvsoc::exec_event_at(int64_t timestamp){
    return gvsoc->step_until(timestamp);
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
    core_requests.push_back(this->get_messy_request_from_gvsoc(req));
}

void AdapterGvsoc::custom_reply(MessyRequest* req){
    vp::io_req *vp_req = (vp::io_req *)req->handle_c;
    vp_req->get_resp_port()->resp(vp_req);
    delete req;
}

void AdapterGvsoc::grant(gv::Io_request *req)
{

}

void AdapterGvsoc::reply(gv::Io_request *req)
{

}

void AdapterGvsoc::notify_stop(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->release();
}

void AdapterGvsoc::notify_run(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->retain();
}