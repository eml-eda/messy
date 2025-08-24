#include <systemc.h>
#include <systemc-ams.h>
#include <config.hpp>
#include <core.hpp>

#define V_CORE ${vref}


/**
 * @class Core_power
 * @brief This module models the power consumption of the core in a SystemC-AMS environment.
 * 
 * This TDF module calculates the voltage and current states of the core based on functional signals
 * and sends the results to the power bus. It interacts with the functional core model through the
 * input `func_signal` and produces the `voltage_state` and `current_state` outputs.
 */
SCA_TDF_MODULE(Core_power)
{
    Core* core; /**< Pointer to the Core functional module. */
    //Data from Functional Instance
    sca_tdf::sc_in  <double> i_power_state; /**< Input port for receiving functional signals from the Core. */
    //Data to Power Bus
    sca_tdf::sca_out <double> o_voltage_a; /**< Output port for sending the voltage state to the Power Bus. */
    sca_tdf::sca_out <double> o_current_a; /**< Output port for sending the current state to the Power Bus. */
    //sca_tdf::sca_out <int> power_to_therm;

    /**
     * @brief Constructor for the Core_power module.
     * 
     * Initializes the input and output signal names for tracing and connecting to other modules.
     */
    SCA_CTOR(Core_power) {}

    /**
     * @brief Sets the attributes of the Core_power module.
     *
     * This function sets the rate and timestep of the `func_signal` port.
     * Check the SystemC-AMS documentation for more details on the `set_rate` and `set_timestep` functions.
     */
    void set_attributes();
    /**
     * @brief Initializes the Core_power module.
     *
     * This function is called once at the beginning of the simulation to initialize the module.
     */
    void initialize();
    /**
     * @brief Processes the Core_power module's functionality.
     *
     * This function is called at each timestep to process the functional signal and calculate
     * the corresponding voltage and current states.
     */
    void processing();

    Core_power(){}
};