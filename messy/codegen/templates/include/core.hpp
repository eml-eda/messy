#ifndef __CORE_H__GVSYSC
#define __CORE_H__GVSYSC
#include "config.h"
#include <systemc.h>
#include <gv/gvsoc.hpp>
#include <cstring>
#include <string.h>
#include <vp/launcher.hpp>
#include <vp/itf/io.hpp>
#include <adapters/${adapter_filenames}.hpp>

class Core : public sc_module
{
    public:
    void run();
    void close();
    void run_next_sc();
    void continue_messy();
    void request_delay(double start_time,int time_to_skip,int resolution);
    void handle_req(messy::Request *req);
    // This gets called when an access from gvsoc side is reaching us
    void access_request(messy::Request *req);
    // This gets called when one of our access gets granted
    void grant_req(messy::Request *req);
    // This gets called when one of our access gets its response
    void reply_to_req(messy::Request *req);
    int read_power=0;
    double tot_power=0.0;
    sc_core::sc_out <int> request_address;
    sc_core::sc_out <int> request_data;
    sc_core::sc_out <bool> F_Out;
    sc_core::sc_out <bool> request_ready;
    sc_core::sc_in  <bool> request_go;
    sc_core::sc_in  <int> request_value;
    //Power Port
    sc_core::sc_out <double> power_signal;

    SC_CTOR(Core):
    request_address("Address_From_Core_to_Func_Bus"),
    request_data("Data_From_Core_to_Func_Bus"),
    F_Out("Flag_From_Core_to_Func_Bus"),
    request_ready("Master_Ready_to_Func_Bus"),
    request_go("Master_GO_to_Func_Bus"),
    request_value("Data_form_Bus_to_Master"),
    power_signal("Func_to_Power_signal")
    {
        SC_THREAD(run);
        sensitive << Go;

    }

    Core(){}

    private:
    int64_t next_timestamp=0;
    int64_t sim_resolution_val=100000;
    ${adapter_class} *iss_adapter;
};
#endif