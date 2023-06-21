#include <systemc.h>
#include <systemc-ams.h>

SCA_TDF_MODULE(air_quality_sensor_power)
{
    sc_core::sc_event idle;
    sca_tdf::sc_in  <int> func_signal;
    //sca_tdf::sc_out <int> power_to_therm;

    SCA_CTOR(air_quality_sensor_power):
        func_signal("State_of_Power_From_Functional")
       // power_to_therm("State_of_Power_to_Therm")
    {}

    void set_attributes();
    void initialize();
    void processing();

    air_quality_sensor_power(){}
};