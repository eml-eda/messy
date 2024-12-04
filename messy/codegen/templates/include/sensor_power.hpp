#include <systemc.h>
#include <systemc-ams.h>
#include <config.hpp>
#define ${sensor_name}_VREF ${vref}

/**
 * @brief Sensor power module class for ${sensor_name}.
 * 
 * This module models the power consumption of the ${sensor_name} sensor. It reads
 * a functional signal and outputs the corresponding voltage and current state
 * to the power bus.
 */
SCA_TDF_MODULE(Sensor_${sensor_name}_power)
{

    sca_tdf::sc_in  <int> func_signal; //< Input port for receiving data from the functional instance.
    sca_tdf::sca_out <double> voltage_state; //< Output port for sending voltage state to the power bus.
    sca_tdf::sca_out <double> current_state; //< Output port for sending current state to the power bus.

    /**
     * @brief Constructor for the Sensor_${sensor_name}_power module.
     * 
     * Initializes the port names for power state signals.
     */
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