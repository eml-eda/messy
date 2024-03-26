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
    #if NUM_SOURCES>0
    sca_tdf::sca_in <double>    current_sources[NUM_SOURCES];
    #endif
    #if NUM_BATTERIES>0
    sca_tdf::sca_out <double>   current_batteries[NUM_BATTERIES];
    #endif

    SCA_CTOR(Power_bus):
    core_current("Current_of_CPU"),
    core_voltage("Voltage_of_CPU")
    {}

    void set_attributes();
    void initialize();
    void processing();

    private:
    double total_current = 0;

    Power_bus(){}
};