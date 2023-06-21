#include "config.h"
#include <systemc.h>

SC_MODULE(Core)
{
    sc_core::sc_out <int> A_Out;
    sc_core::sc_out <int> D_Out;
    sc_core::sc_out <bool> F_Out;
    sc_core::sc_in  <int> Data_in;

    SC_CTOR(Core):
    A_Out("Address_From_Core_to_Func_Bus"),
    D_Out("Data_From_Core_to_Func_Bus"),
    F_Out("Flag_From_Core_to_Func_Bus"),
    Data_in("Data_form_Bus_to_Master")
    {
        SC_THREAD(Simulation);
        sensitive << Data_in;

    }

    void Simulation();

    Core(){}
};