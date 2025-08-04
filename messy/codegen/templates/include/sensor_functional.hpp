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
 *  - i_is_active: Input port indicating if the sensor is i_is_active to be used.
 * - Output Ports:
 *  - data_out: Output port for the data read from the sensor's memory.
 *  - go: Output port for signaling the sensor to proceed with the operation.
 *  - power_signal: Output port for the power state signal
 */
SC_MODULE(Sensor_${sensor_name}_functional) {
    Core *core;

    // Input Port
    sc_core::sc_in<bool> i_is_enabled; ///< Input port for enabling the sensor.
    sc_core::sc_in<unsigned int> i_address; ///< Input port for the memory address. When writing this is the address to write to, when reading this is the address to read from.
    sc_core::sc_in<uint8_t *> i_data_ptr; ///< Input port for incoming data to be written to the sensor's memory.
    sc_core::sc_in<unsigned int> i_size; ///< Input port for the size of the request. It must be coehrent with the size of the data_in.
    sc_core::sc_in<bool> i_is_read; ///< Input port for the write/read flag (true for read, false for write).
    sc_core::sc_in<bool> i_is_active; ///< Input port indicating if the system is i_is_active.

    // Output Port
    sc_core::sc_out<uint8_t *> o_data_ptr; ///< Output port for the data read from the sensor's memory.
    sc_core::sc_out<bool> o_is_done; ///< Output port for signaling the sensor to proceed with the operation.
    
    // Power Port
    sc_core::sc_out<int> o_power_state; ///< Output port for the power state signal.
    
    // Thermal Port
    // sc_core::sc_out <int> thermal_signal;

    
    /**
     * @brief Constructor for the Sensor_${sensor_name}_functional module.
     * 
     * Initializes input and output ports and sets up the sensor logic thread.
     * It also allocates memory for the sensor's register map.
     */
    SC_CTOR(Sensor_${sensor_name}_functional) {
        // printf("SENSOR :: systemc constructor");

        _register_memory_ptr = new uint8_t[${register_memory}];  ///< Allocate memory for the sensor's register map.

        // Declare the sensor logic thread and make it sensitive to the 'i_is_active' signal.
        SC_THREAD(sensor_logic);
        sensitive << i_is_active;
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
        
    }

    /**
     * @brief Destructor for Sensor_${sensor_name}_functional.
     * 
     * Frees the dynamically allocated memory for the sensor's register map.
     */
    ~Sensor_${sensor_name}_functional() {
        delete[] _register_memory_ptr;
    }

    // Register Map
  private:
    uint8_t *_register_memory_ptr; ///< Pointer to the sensor's register memory.
    int _register_memory_size = ${register_memory}; ///< Size of the sensor's register memory.
};