#include <config.h>
#include <systemc.h>

SC_MODULE(rf_functional)
{
    //Input Port
    sc_core::sc_in <bool> enable;
    sc_core::sc_in <int>  address;
    sc_core::sc_in <int>  data_in;
    sc_core::sc_in <bool> flag_wr;
    //Output Port
    sc_core::sc_out <int> data_out;
    //Power Port
    sc_core::sc_out <int> power_signal;
    //Thermal Port
    //sc_core::sc_out <int> thermal_signal;
    
    SC_CTOR(rf_functional): 
    enable("Enable_signal"),
    address("Address"),
    data_in("Data_in"),
    flag_wr("Flag"),
    data_out("Data_out"),
    power_signal("Func_to_Power_signal")
    {    
        SC_THREAD(sensor_logic);
        sensitive << enable 
                  << address 
                  << data_in 
                  << flag_wr;
    }

    void sensor_logic();

    rf_functional(){}
    //Register Map
    private: 
    int Register[RF_REG_DIMENSION];
};