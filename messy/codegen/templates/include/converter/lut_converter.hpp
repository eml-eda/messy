#include <systemc-ams.h>
#include <config.hpp>
#include <lut.hpp>

/**
 * @def SIZE_CONV_DCDC_${name}
 * @brief Defines the size of the lookup table for the converter.
 */
#define SIZE_CONV_DCDC_${name} ${len(lut["input_values"])}

/**
 * @brief Lookup table for input values of the converter.
 * 
 * This static array contains the input values for the converter's lookup table.
 */
static const double ${input_variable}_CONV_${name}[SIZE_CONV_DCDC_${name}] = {${str(lut["input_values"])[1:len(str(lut["input_values"]))-1]}};

/**
 * @brief Lookup table for efficiency values of the converter.
 * 
 * This static array contains the efficiency values corresponding to the input values
 * for the converter's lookup table.
 */
static const double ETA_CONV_${name}[SIZE_CONV_DCDC_${name}] = {${str(lut["efficiency_values"])[1:len(str(lut["efficiency_values"]))-1]}};


/**
 * @class ${name}_converter
 * @brief SystemC-AMS module representing a converter.
 * 
 * The ${name}_converter module models a converter that takes current and voltage inputs
 * from the ${name} and outputs the current delivered to the BUS. It uses lookup tables
 * to determine the efficiency of the conversion process, and in fact it contains a LUT object as a private variable.
 */
SCA_TDF_MODULE(${name}_converter)
{
    // Input Ports
    sca_tdf::sca_in<double> current_in; /**< Input current from the ${'core_power' if name == 'core' else 'power_bus' if name == 'battery' else 'unknown_source'}. ${'The input current is the current coming from the power_bus that should go to charge or discharge the battery, but before going to the battery, it passes through the battery_converter.' if name == 'battery' else ''} */
    sca_tdf::sca_in<double> voltage_in; ///< Input voltage from the ${'core_power' if name == 'core' else name}

    // Output Port
    sca_tdf::sca_out<double> current_out; /**< Output current to the ${'power_bus' if name == 'core' else 'battery' if name == 'battery' else 'unknown_source'} */

    /**
     * @brief Constructor for ${name}_converter.
     * 
     * Initializes the input and output ports with their respective names.
     */
    SCA_CTOR(${name}_converter): current_in("current_in"),
                            voltage_in("voltage_in"),
                            current_out("current_out") {};

    /**
     * @brief Sets the attributes of the converter.
     * 
     * This function sets the timestep and delay for the `current_out` port.
     */
    void set_attributes();
    /**
     * @brief Initializes the converter.
     * 
     * This function is called once at the beginning of the simulation to initialize the converter.
     */
    void initialize();
    /**
     * @brief Processes the converter's functionality.
     * 
     * This function is called at each timestep to process the input signals and generate the output.
     */
    void processing();

    /**
     * @brief Constructor for ${name}_converter.
     * 
     * Initializes the input and output ports with their respective names.
     */
    ${name}_converter(){}

    private:
        /**
         * @brief Lookup table for efficiency values.
         * 
         * This LUT object is used to store and retrieve efficiency values based on the input values.
         */
        LUT lut_eta = LUT(${input_variable}_CONV_${name}, ETA_CONV_${name}, SIZE_CONV_DCDC_${name});
};
