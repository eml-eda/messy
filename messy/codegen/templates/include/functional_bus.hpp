#include <config.hpp>
#include <systemc.h>

/**
 * @class Functional_bus
 * @brief SystemC module representing a functional bus.
 *
 * The Functional_bus module handles communication between a master and multiple sensors.
 * It processes requests from the master and routes data to and from the sensors.
 */
SC_MODULE(Functional_bus) {

    // Input Port
    sc_core::sc_in<unsigned int> i_address; /**< Address of the request from the master */
    sc_core::sc_in<uint8_t *> i_data_ptr; /**< Data of the request from the master */
    sc_core::sc_in<bool> i_is_read; /**< Flag indicating the type of request from the master */
    sc_core::sc_in<bool> i_is_active; /**< Flag indicating if the request is ready */
    sc_core::sc_in<unsigned int> i_size;  /**< Size of the request from the master */
    sc_core::sc_in<uint8_t *> i_data_sensor_ptr[NUM_SENSORS];  /**< Data input from sensors */
    sc_core::sc_in<bool> i_is_done_sensors[NUM_SENSORS];  /**< Size of the request from the master */

    // Output Port
    sc_core::sc_out<uint8_t *> o_data_ptr; /**< Data of the response to the master */
    sc_core::sc_out<bool> o_is_done; /**< Flag indicating if the bus is ready to process the request */
    sc_core::sc_out<int> o_idx_sensor; /**< Index of the selected sensor */
    sc_core::sc_out<unsigned int> o_address[NUM_SENSORS]; /**< Address output to sensors */
    sc_core::sc_out<uint8_t *> o_data_sensors_ptr[NUM_SENSORS]; /**< Data output to sensors */
    sc_core::sc_out<unsigned int> o_size[NUM_SENSORS]; /**< Size of the data output to sensors */
    sc_core::sc_out<bool> o_is_read[NUM_SENSORS]; /**< Flags indicating the type of data output to sensors */
    sc_core::sc_out<bool> o_activate_sensors[NUM_SENSORS]; /**< Flags indicating if sensors are ready */


    /**
     * @brief Constructor for Functional_bus.
     * 
     * Initializes the input and output ports and sets up the sensitivity list for the SC_THREAD.
     */
    SC_CTOR(Functional_bus) {
        SC_THREAD(processing_data);
        sensitive << i_is_active;
        for (int i = 0; i < NUM_SENSORS; i++) {
            sensitive << i_is_done_sensors[i];
        }
    }

    /**
     * @brief Thread for processing data.
     * 
     * This function is responsible for processing the data received from the master
     * and routing it to the appropriate sensor.
     */
    void processing_data();
      
    /**
     * @brief Function for handling responses.
     * 
     * This function is responsible for handling the responses from the sensors
     * and sending them back to the master.
     */
    void response();

  private:
    int _selected_sensor = 0; /**< Index of the currently selected sensor */

    Functional_bus() {} /**< Private default constructor to prevent direct instantiation without named ports. */
};