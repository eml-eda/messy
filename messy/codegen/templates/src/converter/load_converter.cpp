#include <converter/load_converter.hpp>

void Load_converter::set_attributes() {

    current_in.set_rate(1);
    current_in.set_timestep(1, SIM_RESOLUTION);
    voltage_in.set_rate(1);
    voltage_in.set_timestep(1, SIM_RESOLUTION);
}

void Load_converter::initialize() {}

/**
 * @warning This function is called by the SystemC-AMS kernel. Do not call it manually. 
 * 
 * @brief Processes the data.
 * 
 * This function is responsible for processing the input data and generating
 * the output data. It reads the current and voltage inputs, calculates the
 * output current based on the efficiency factor and input values, and writes
 * the output current to the current_out port.
 */
void Load_converter::processing() {
    double v_tmp; /**< Temporary variable to store the input voltage */
    double i_tmp; /**< Temporary variable to store the input current */
    double i_out; /**< Variable to store the calculated output current */

    // Read input quantities
    v_tmp = voltage_in.read(); /**< Read the voltage input from the port */
    i_tmp = current_in.read(); /**< Read the current input from the port */

    // Compute the output current (Psensor * efficiency / Vbus)
    if (v_tmp == 0) {
        i_out = 0.0; /**< Set output current to 0 if input voltage is 0 */
    } else {
        i_out = ( (v_tmp * i_tmp) * efficency_factor ) / VREF_BUS; /**< Calculate the output current based on input values and efficiency factor */
    }

    current_out.write(i_out); /**< Write the calculated output current to the port */
}

/**
 * @brief Sets the efficiency factor of the converter.
 * 
 * This function sets the efficiency factor used in the conversion process.
 * 
 * @param k The efficiency factor to be set.
 */
void Load_converter::set_efficency(double k) {
    efficency_factor = k;
}