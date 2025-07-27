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
    sc_core::sc_in<unsigned int> request_address; /**< Address of the request from the master */
    sc_core::sc_in<uint8_t *> request_data; /**< Data of the request from the master */
    sc_core::sc_in<bool> flag_from_core; /**< Flag indicating the type of request from the master */
    sc_core::sc_in<bool> request_ready; /**< Flag indicating if the request is ready */
    sc_core::sc_in<unsigned int> request_size;  /**< Size of the request from the master */
    sc_core::sc_in<uint8_t *> data_input_sensor[NUM_SENSORS];  /**< Data input from sensors */
    sc_core::sc_in<bool> go_sensors[NUM_SENSORS];  /**< Size of the request from the master */

    // Output Port
    sc_core::sc_out<uint8_t *> request_value; /**< Data of the response to the master */
    sc_core::sc_out<bool> request_go; /**< Flag indicating if the bus is ready to process the request */
    sc_core::sc_out<int> idx_sensor; /**< Index of the selected sensor */
    sc_core::sc_out<unsigned int> address_out_sensor[NUM_SENSORS]; /**< Address output to sensors */
    sc_core::sc_out<uint8_t *> data_out_sensor[NUM_SENSORS]; /**< Data output to sensors */
    sc_core::sc_out<unsigned int> size_out_sensor[NUM_SENSORS]; /**< Size of the data output to sensors */
    sc_core::sc_out<bool> flag_out_sensor[NUM_SENSORS]; /**< Flags indicating the type of data output to sensors */
    sc_core::sc_out<bool> ready_sensor[NUM_SENSORS]; /**< Flags indicating if sensors are ready */


    /**
     * @brief Constructor for Functional_bus.
     * 
     * Initializes the input and output ports and sets up the sensitivity list for the SC_THREAD.
     */
    SC_CTOR(Functional_bus) : request_address("Address_from_Master_to_Bus"),
                              request_data("Data_from_Master_to_Bus"),
                              flag_from_core("Flag_from_Master_to_Bus"),
                              request_ready("Ready_from_Master_to_Bus"),
                              request_value("Data_from_Bus_to_Master"),
                              request_go("Go_from_Bus_to_Master"),
                              idx_sensor("selected_sensor_from_request") {
        SC_THREAD(processing_data);
        sensitive << request_ready;
        for (int i = 0; i < NUM_SENSORS; i++) {
            sensitive << go_sensors[i];
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
    int selected_sensor = 0; /**< Index of the currently selected sensor */

    Functional_bus() {} /**< Private default constructor to prevent direct instantiation without named ports. */
};