#include "config.h"
#include <systemc.h>
#include <gv/gvsoc.hpp>
#include <cstring>
#include <string.h>

class Core : public gv::Io_user, public sc_module
{
    public:
    void run();

    // This gets called when an access from gvsoc side is reaching us
    void access(gv::Io_request *req);
    // This gets called when one of our access gets granted
    void grant(gv::Io_request *req);
    // This gets called when one of our access gets its response
    void reply(gv::Io_request *req);

    sc_core::sc_out <int> A_Out;
    sc_core::sc_out <int> D_Out;
    sc_core::sc_out <bool> F_Out;
    sc_core::sc_out <bool> Ready;
    sc_core::sc_in  <bool> Go;
    sc_core::sc_in  <int> Data_in;
    //Power Port
    sc_core::sc_out <int> power_signal;

    SC_CTOR(Core):
    A_Out("Address_From_Core_to_Func_Bus"),
    D_Out("Data_From_Core_to_Func_Bus"),
    F_Out("Flag_From_Core_to_Func_Bus"),
    Ready("Master_Ready_to_Func_Bus"),
    Go("Master_GO_to_Func_Bus"),
    Data_in("Data_form_Bus_to_Master"),
    power_signal("Func_to_Power_signal")
    {
        SC_THREAD(run);
        sensitive << Go;

    }

    Core(){}

    private:
    gv::Io_binding *axi;
    gv::Gvsoc *gvsoc;
    gv::Io_request *req_global;
};