#include <systemc.h>
#include <systemc-ams.h>
#include "config.h"

SCA_TDF_MODULE(Sensor_${sensor_name}_power)
{
    //Data from Functional Instance
    sca_tdf::sc_in  <int> func_signal;
    //Data to Power Bus
    sca_tdf::sca_out <double> voltage_state;
    sca_tdf::sca_out <double> current_state;
    //sca_tdf::sc_out <int> power_to_therm;

    SCA_CTOR(Sensor_${sensor_name}_power):
        func_signal("State_of_Power_From_Functional"),
        voltage_state("Voltage_trace_to_Power_Bus"),
        current_state("Current_trace_to_Power_Bus")
    {}

    void set_attributes();
    void initialize();
    void processing();

    Sensor_${sensor_name}_power(){}
};