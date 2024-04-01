#include <adapters/adapter_gvsoc_gap9.hpp>

AdapterGvsocGap9::AdapterGvsocGap9()
{
}
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

MessyRequest* AdapterGvsocGap9::get_messy_request_from_gvsoc(gv::Io_request* req){
    return (MessyRequest*)new MessyRequest((long long)req->addr,(unsigned int*)req->data,(bool)req->type==gv::Io_request_read,(unsigned int*)req->handle);
}

int64_t AdapterGvsocGap9::exec_events_at(int64_t timestamp){
    int64_t next_event_time=gvsoc->step_until(timestamp);
    finished=this->gvsoc->top_get()->time_engine->finished_get();
    return next_event_time;
}

double AdapterGvsocGap9::get_power_at(int64_t timestamp){
    double x=0,y=0;
    double inst_power=this->gvsoc->get_instant_power(x,y);
    return inst_power;
}

void AdapterGvsocGap9::close(){
    if(!closed){
        int retval = gvsoc->join();
        gvsoc->stop();
        gvsoc->close();
        closed=1;
    }
}

void AdapterGvsocGap9::access(gv::Io_request* req){
    add_request(this->get_messy_request_from_gvsoc(req));
}

void AdapterGvsocGap9::custom_reply(MessyRequest* req){
    vp::io_req *vp_req = (vp::io_req *)req->handle_c;
    vp_req->get_resp_port()->resp(vp_req);
    delete req;
}

void AdapterGvsocGap9::grant(gv::Io_request *req)
{

}

void AdapterGvsocGap9::reply(gv::Io_request *req)
{

}

void AdapterGvsocGap9::notify_stop(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->release();
}

void AdapterGvsocGap9::notify_run(int64_t time)
{
    if(!this->gvsoc->retain_count())
        this->gvsoc->retain();
}