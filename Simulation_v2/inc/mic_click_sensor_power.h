#include <systemc.h>
#include <systemc-ams.h>

SCA_TDF_MODULE(mic_click_sensor_power)
{
    //Data from Functional Instance
    sca_tdf::sc_in  <int> func_signal;
    //Data to Power Bus
    sca_tdf::sca_out <double> voltage_state;
    sca_tdf::sca_out <double> current_state;
    //sca_tdf::sc_out <int> power_to_therm;

    SCA_CTOR(mic_click_sensor_power):
        func_signal("State_of_Power_From_Functional"),
        voltage_state("Voltage_trace_to_Power_Bus"),
        current_state("Current_trace_to_Power_Bus")
       // power_to_therm("State_of_Power_to_Therm")
    {}

    void set_attributes();
    void initialize();
    void processing();

    mic_click_sensor_power(){}
};