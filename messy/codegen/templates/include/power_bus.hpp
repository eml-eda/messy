#include <config.hpp>
#include <systemc-ams.h>
#include <systemc.h>

SCA_TDF_MODULE(Power_bus) {
    // Time resolution is set to 1 ms, meaning the module is triggered every 1 ms

    // Input Ports
    sca_tdf::sca_in<double> core_current;
    sca_tdf::sca_in<double> core_voltage;
    sca_tdf::sca_in<double> voltage_sensors[NUM_SENSORS];
    sca_tdf::sca_in<double> current_sensors[NUM_SENSORS];

#if NUM_SOURCES > 0
    sca_tdf::sca_in<double> current_sources[NUM_SOURCES];
#endif
#if NUM_BATTERIES > 0
    sca_tdf::sca_out<double> current_batteries[NUM_BATTERIES];
#endif

    /**
     * @brief Constructor for the Core_power module.
     * 
     * Initializes the input and output signal names for tracing and connecting to other modules.
     */
    SCA_CTOR(Power_bus) : core_current("Current_of_CPU"),
                          core_voltage("Voltage_of_CPU") {}

    void set_attributes();
    void initialize();
    void processing();

  private:
    double total_current = 0;

    // Default constructor is private to prevent its use, ensuring that
    // the module must be initialized using the public constructor above.
    Power_bus() {}
};