#include <config.hpp>
#include <systemc-ams.h>

/**
 * @class Load_converter
 * @brief SystemC-AMS module representing a load converter.
 *
 * The Load_converter module models a converter that takes current and voltage inputs
 * and outputs the current delivered to the power bus. It includes methods for setting
 * attributes, initialization, processing, and setting the efficiency factor.
 */
SCA_TDF_MODULE(Load_converter) {
    // Input Port
    sca_tdf::sca_in<double> current_in;  /**< Input current from the power instance */
    sca_tdf::sca_in<double> voltage_in; /**< Input voltage from the power instance */

    // Output Port
    sca_tdf::sca_out<double> current_out; /**< Output current to the power bus */

    /**
     * @brief Constructor for Load_converter.
     *
     * Initializes the input and output ports with their respective names.
     */
    SCA_CTOR(Load_converter) : current_in("Current_from_power_instance"),
                               voltage_in("Voltage_from_power_instance"),
                               current_out("Current_to_power_bus") {}

    void set_attributes();
    void initialize();
    void processing();

    /**
     * @brief Sets the efficiency factor of the converter.
     *
     * This function sets the efficiency factor used in the conversion process.
     *
     * @param k The efficiency factor to be set.
     */
    void set_efficency(double k);

    Load_converter() {}

  private:
    double efficency_factor = 1.0; /**< Efficiency factor for the conversion process */
};