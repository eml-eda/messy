#include <core_power.hpp>

/**
 * @warning This function should not be called directly. It is called by the SystemC-AMS kernel.
 * 
 * @brief Sets the attributes of the `func_signal` port.
 * 
 * This function sets the rate and timestep of the `func_signal` port. Check the SystemC-AMS documentation for more details on the `set_rate` and `set_timestep` functions.
 */
void Core_power::set_attributes() {
    func_signal.set_rate(1); ///< Set the number of samples that have to be read on the port per execution. Check SystemC-AMS documentation for more details on the set_rate function.
    func_signal.set_timestep(1, SIM_RESOLUTION); //< Set how often the module is triggered (sampling period). Check SystemC-AMS documentation for more details on the set_timestep function.
}

/**
 * @brief Initializes the Core_power module.
 *
 * This function is called once at the beginning of the simulation to initialize the module.
 * Currently, no specific initialization is required.
 */
void Core_power::initialize() {}

/**
 * @warning This function should not be called directly. It is called by the SystemC-AMS kernel.
 * 
 * @brief Main processing function for the Core_power module.
 *
 * This function calculates the voltage and current states based on the functional signal.
 * If the core has not performed any simulation iterations, it writes 0.0 to the voltage and
 * current outputs. Otherwise, it calculates the current state based on the total power
 * consumed and the number of simulation iterations, and resets the power-related parameters.
 */
void Core_power::processing() {
    // If no simulation iterations have been performed, set voltage and current to 0.
    if (!core->simulation_iters) {
        voltage_state.write(0.0);
        current_state.write(0.0);
        return;
    }

    // Calculate and write the current state based on total power and number of iterations.
    voltage_state.write(V_CORE);
    current_state.write(((core->tot_power / core->simulation_iters) * 1000) / V_CORE);

    // Reset the core's simulation iteration count and total power.
    core->simulation_iters = 0;
    core->tot_power = 0;
}
