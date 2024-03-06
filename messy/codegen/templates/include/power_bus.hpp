#include <systemc.h>
#include <systemc-ams.h>
#include "config.h"

SCA_TDF_MODULE(power_bus){
    //Rate 1ms
    //Input Port
    sca_tdf::sca_in <double>  core_current;
    sca_tdf::sca_in <double>  core_voltage;
    sca_tdf::sca_in <double>  voltage_sensors[NP];
    sca_tdf::sca_in <double>  current_sensors[NP];

    //Output Port
    sca_tdf::sca_out <double>  battery_out_current;

    SCA_CTOR(power_bus):
    core_current("Current_of_CPU"),
    core_voltage("Voltage_of_CPU"),
    battery_out_current("Current_to_Battery")
    {}

    void set_attributes();
    void initialize();
    void processing();

    private:
    double total_current = 0;

    power_bus(){}
};