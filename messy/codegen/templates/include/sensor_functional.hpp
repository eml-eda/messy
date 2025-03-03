#include <config.hpp>
#include <core.hpp>
#include <systemc.h>

/**
 * @brief Functional module for the ${sensor_name} sensor.
 *
 * This module handles the functional behavior of the ${sensor_name} sensor. It processes data from input ports, controls sensor operations through internal logic, and interacts with power and thermal models.
 * 
 * The module includes the following ports:
 * - Input Ports:
 *  - enable: Input port for enabling the sensor.
 *  - address: Input port for the memory address. When writing, this is the address to write to; when reading, this is the address to read from.
 *  - data_in: Input port for incoming data to be written to the sensor's memory.
 *  - req_size: Input port for the size of the request.
 *  - flag_wr: Input port for the write/read flag (true for read, false for write).
 *  - ready: Input port indicating if the sensor is ready to be used.
 * - Output Ports:
 *  - data_out: Output port for the data read from the sensor's memory.
 *  - go: Output port for signaling the sensor to proceed with the operation.
 *  - power_signal: Output port for the power state signal
 */
SC_MODULE(Sensor_${sensor_name}_functional) {
    Core *core;

    // Input Port
    sc_core::sc_in<bool> enable; ///< Input port for enabling the sensor.
    sc_core::sc_in<unsigned int> address; ///< Input port for the memory address. When writing this is the address to write to, when reading this is the address to read from.
    sc_core::sc_in<uint8_t *> data_in; ///< Input port for incoming data to be written to the sensor's memory.
    sc_core::sc_in<unsigned int> req_size; ///< Input port for the size of the request. It must be coehrent with the size of the data_in.
    sc_core::sc_in<bool> flag_wr; ///< Input port for the write/read flag (true for read, false for write).
    sc_core::sc_in<bool> ready; ///< Input port indicating if the system is ready.

    // Output Port
    sc_core::sc_out<uint8_t *> data_out; ///< Output port for the data read from the sensor's memory.
    sc_core::sc_out<bool> go; ///< Output port for signaling the sensor to proceed with the operation.
    
    // Power Port
    sc_core::sc_out<int> power_signal; ///< Output port for the power state signal.
    
    // Thermal Port
    // sc_core::sc_out <int> thermal_signal;

    
    /**
     * @brief Constructor for the Sensor_${sensor_name}_functional module.
     * 
     * Initializes input and output ports and sets up the sensor logic thread.
     * It also allocates memory for the sensor's register map.
     */
    SC_CTOR(Sensor_${sensor_name}_functional) : enable("Enable_signal"),
                                                address("Address"),
                                                data_in("Data_in"),
                                                flag_wr("Flag"),
                                                ready("Ready"),
                                                data_out("Data_out"),
                                                go("Go"),
                                                power_signal("Func_to_Power_signal") {
        // printf("SENSOR :: systemc constructor");

        register_memory = new uint8_t[${register_memory}];  ///< Allocate memory for the sensor's register map.

        // Declare the sensor logic thread and make it sensitive to the 'ready' signal.
        SC_THREAD(sensor_logic);
        sensitive << ready;
    }

    
    /**
     * @brief Thread that implements the sensor logic.
     * 
     * This function controls the internal sensor operations based on the input signals
     * and manages the interaction between data, address, and control flags.
     */
    void sensor_logic();

    /**
     * @brief Default constructor for Sensor_${sensor_name}_functional.
     * 
     * A placeholder constructor
     */
    Sensor_${sensor_name}_functional() {
        // printf("SENSOR :: constructor");
    }

    /**
     * @brief Destructor for Sensor_${sensor_name}_functional.
     * 
     * Frees the dynamically allocated memory for the sensor's register map.
     */
    ~Sensor_${sensor_name}_functional() {
        delete[] register_memory;
    }

    // Register Map
  private:
    uint8_t *register_memory; ///< Pointer to the sensor's register memory.
    int register_memory_size = ${register_memory}; ///< Size of the sensor's register memory.
};