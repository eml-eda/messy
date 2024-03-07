#include <systemc.h>
#include <systemc-ams.h>
#include <config.hpp>

SCA_TDF_MODULE(Power_bus){
    //Rate 1ms
    //Input Port
    sca_tdf::sca_in <double>  core_current;
    sca_tdf::sca_in <double>  core_voltage;
    sca_tdf::sca_in <double>  voltage_sensors[NUM_SENSORS];
    sca_tdf::sca_in <double>  current_sensors[NUM_SENSORS];

    //Output Port
    sca_tdf::sca_out <double>  battery_out_current;

    SCA_CTOR(Power_bus):
    core_current("Current_of_CPU"),
    core_voltage("Voltage_of_CPU"),
    battery_out_current("Current_to_Battery")
    {}

    void set_attributes();
    void initialize();
    void processing();

    private:
    double total_current = 0;

    Power_bus(){}
};