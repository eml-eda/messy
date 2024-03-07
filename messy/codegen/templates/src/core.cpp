#include <core.hpp>

void Core::run()
{
    iss_adapter->startup();
    
    while(!this->iss_adapter->finished)
        continue_messy();
    // wait until next resolutional value(next ms)
    wait(sim_resolution_val-(next_timestamp%sim_resolution_val),sc_core::SC_PS);
}

void Core::run_next_sc(){
    wait(next_timestamp-sc_time_stamp().to_double(),sc_core::SC_PS);
    simulation_iters++;
}

void Core::close(){
    this->iss_adapter->close();
    fprintf(stdout, "End simulation at %ld\n",next_timestamp);
    sc_stop();
}

void Core::handle_req_queue(){}

void Core::continue_messy(){

    next_timestamp = this->iss_adapter->exec_event_at(sc_timestamp);

    this->handle_req_queue();
        
    this->run_next_sc();

}


void Core::access_request(MessyRequest *req){

}

void Core::handle_req(MessyRequest *req)
{
    //Here connect the signals with SystemC simulator signals   
    //std::cout << sc_time_stamp().to_double() << std::endl;
    int tmpaddr;
    if (req->read_req)
    {

        request_data.write(1);
        F_Out.write(true);
        request_ready.write(true);
        tmpaddr = req->addr;
        
        request_address.write(tmpaddr);
        wait();
        request_ready.write(false);
        wait();

    }
    else
    {
        request_data.write(*(req->data));
        F_Out.write(false);
        request_ready.write(true);
        tmpaddr = req->addr;

        request_address.write(tmpaddr);

        wait();
        request_ready.write(false);
        wait();
        
    }
    *((uint32_t*)req->data) = request_value.read();
    iss_adapter->custom_reply(req);
}


void Core::request_delay(double start_time,int time_to_skip,int resolution){
    double time=(time_to_skip*sim_resolution_val)+start_time;
    while(next_timestamp<time)
        this->continue_messy();
}

void Core::grant_req(MessyRequest *req)
{

}

void Core::reply_to_req(MessyRequest *req)
{

}