#include <converter/${name}_converter.hpp>


/**
 * @brief Sets the attributes of the converter.
 * 
 * This function sets the timestep and delay for the `current_out` port.
 */
void ${name}_converter::set_attributes()
{
    current_out.set_timestep(1, SIM_RESOLUTION);
    current_out.set_delay(1);
}

/**
 * @brief Initializes the converter.
 * 
 * This function is called once at the beginning of the simulation to initialize the converter.
 */
void ${name}_converter::initialize() {}


/**
 * @warning This function is called by the SystemC-AMS scheduler. Do not call it manually.
 * 
 * @brief Processes the data.
 * 
 * This function is responsible for processing the input data and generating
 * the output data. It reads the current and voltage inputs, calculates the
 * efficiency using the lookup table, and computes the current output based
 * on the efficiency and input values.
 */
void ${name}_converter::processing()
{
    double ideal_power;  /**< Variable to store the ideal power */
    double eta; /**< Variable to store the efficiency */
    double i_tmp; /**< Temporary variable to store the input current */
    double v_tmp; /**< Read the voltage input from the port */

    // Read input quantities
    i_tmp = current_in.read(); /**< Read the current input from the port */
    v_tmp = voltage_in.read();  /**< Read the voltage input from the port */

    // Get efficiency
    eta = lut_eta.get_val(${"v_tmp" if input_variable=="voltage" else "i_tmp"}) / 100; /**< Get the efficiency from the lookup table */
    
% if not out_dir:
    /** 
     * The efficiency of this converter is defined as:
     * eta = Pin / Pout
     * where Pin = i_tmp * v_tmp and Pout = current_out * VREF_BUS
     * Therefore, the output current is:
     * current_out = eta * (i_tmp * VREF_BUS) / v_tmp
     */
    current_out.write(i_tmp * v_tmp / (VREF_BUS*eta));
% else:
    /**
     * 
     * The efficiency of this converter is defined as:
     * eta = Pout / Pin
     * where Pin = i_tmp * VREF_BUS and Pout = current_out * v_tmp
     * Therefore, the output current is:
     * current_out = (eta * (i_tmp * VREF_BUS)) / v_tmp
     * 
     */
    current_out.write((eta * (i_tmp * VREF_BUS)) / v_tmp);
% endif
}
