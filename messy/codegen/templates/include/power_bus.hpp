#include <config.hpp>
#include <systemc-ams.h>
#include <systemc.h>

/**
 * @brief Power bus module for managing and aggregating power-related signals.
 * 
 * The Power_bus module models the power consumption of a system by aggregating
 * current and voltage values from various components such as the CPU, sensors,
 * and power sources. The module is triggered every 1 ms to update the power states.
 */
SCA_TDF_MODULE(Power_bus) {
    // Time resolution is set to 1 ms, meaning the module is triggered every 1 ms

    // Input Ports
    sca_tdf::sca_in<double> core_current; ///< Input port for the current consumed by the CPU core.
    sca_tdf::sca_in<double> core_voltage; ///< Input port for the voltage supplied to the CPU core.
    sca_tdf::sca_in<double> voltage_sensors[NUM_SENSORS]; ///< Array of input ports for the voltages supplied to the sensors.
    sca_tdf::sca_in<double> current_sensors[NUM_SENSORS]; ///< Array of input ports for the currents consumed by the sensors.

#if NUM_SOURCES > 0
    sca_tdf::sca_in<double> current_sources[NUM_SOURCES]; //< Array of input ports for the currents supplied by external power sources (i.e. photovoltailc panels).
#endif
#if NUM_BATTERIES > 0
    sca_tdf::sca_out<double> current_batteries[NUM_BATTERIES]; //< Array of output ports for the currents drawn from batteries.
#endif

    /**
     * @brief Constructor for the Core_power module.
     * 
     * Initializes the input and output signal names for tracing and connecting to other modules.
     */
    SCA_CTOR(Power_bus) : core_current("Current_of_CPU"),
                          core_voltage("Voltage_of_CPU") {}

    /**
     * @brief Sets the attributes of the Power_bus module.
     *
     * This function sets the timestep for the input and output signals of the Power_bus module.
     * It ensures that the module is triggered at the specified simulation resolution.
     */
    void set_attributes();
    /**
     * @brief Initializes the Power_bus module.
     *
     * This function is called once at the beginning of the simulation to initialize the module.
     */
    void initialize();
    /**
     * @brief Processes the Power_bus module's functionality.
     *
     * This function is called at each timestep to process the input current and voltage signals
     * from various components and calculate the total current on the power bus.
     */
    void processing();

  private:
    double total_current = 0;

    // Default constructor is private to prevent its use, ensuring that
    // the module must be initialized using the public constructor above.
    Power_bus() {}
};