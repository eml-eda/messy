#include <config.h>
#include <systemc.h>

SC_MODULE(air_quality_sensor_functional)
{
    //Input Port
    sc_core::sc_in <bool> enable;
    sc_core::sc_in <int>  address;
    sc_core::sc_in <int>  data_in;
    sc_core::sc_in <bool> flag_wr;
    sc_core::sc_in <bool> ready;
    //Output Port
    sc_core::sc_out <int> data_out;
    sc_core::sc_out <bool> go;
    //Power Port
    sc_core::sc_out <int> power_signal;
    //Thermal Port
    //sc_core::sc_out <int> thermal_signal;
    
    SC_CTOR(air_quality_sensor_functional): 
    enable("Enable_signal"),
    address("Address"),
    data_in("Data_in"),
    flag_wr("Flag"),
    ready("Ready"),
    data_out("Data_out"),
    go("Go"),
    power_signal("Func_to_Power_signal")
    {    
        SC_THREAD(sensor_logic);
        sensitive << ready;
    }

    void sensor_logic();

    air_quality_sensor_functional(){}
    //Register Map
    private: 
    int Register[AIR_QUALITY_SENSOR_REG_DIMENSION];
};