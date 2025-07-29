#include <config.hpp>
#include <functional_bus.hpp>

/**
 * @brief Thread for processing data.
 * 
 * This function is responsible for processing the data received from the master
 * and routing it to the appropriate sensor. It reads the request details such as
 * address, data, size, and flag from the master, and determines which sensor the
 * request is intended for. If the request is valid, it sets the appropriate output
 * signals to the sensor. If the request is invalid, it logs an error and sets the
 * response to indicate failure.
 */
void Functional_bus::processing_data(){
    unsigned int add_s; /**< Local variable to store the request address */
    uint8_t* data_s; /**< Local variable to store the request data */
    unsigned int reg_s; /**< Local variable to store the register address within the sensor */
    unsigned int size_s; /**< Local variable to store the size of the request */
    bool flag_s; /**< Local variable to store the request flag */
    
    while (true){
        // Check if the request is ready
        if (request_ready.read() == true) {
            // Read the request details from the input ports
            add_s = request_address.read();
            flag_s = flag_from_core.read();
            size_s = request_size.read();

            // Iterate over the sensors to find the matching address range
            % for idx,(sensor_name,sensor) in enumerate(peripherals["sensors"].items()):
            % if idx == 0:
            if
            % else:
            else if
            % endif
            (${sensor_name}_BASE_ADDRESS <= add_s && add_s < (${sensor_name}_BASE_ADDRESS + ${sensor["register_memory"]})) 
            {
                // Check if the request exceeds the sensor's memory range
                if(add_s+size_s > (${sensor_name}_BASE_ADDRESS + ${sensor["register_memory"]})){
                    fprintf(stderr,"\n[MESSY]\tRead/Write at %d of %d bytes exceeds ${sensor_name} memory\n",add_s,size_s);
                    selected_sensor=-1;
                    request_value.write(0x0);
                    request_go.write(true);
                }
                else{
                    // Calculate the register address within the sensor
                    reg_s = add_s - ${sensor_name}_BASE_ADDRESS;
                    data_s = request_data.read();
                    // Set the output signals to the sensor
                    size_out_sensor[${idx}].write(size_s);
                    address_out_sensor[${idx}].write(reg_s);
                    data_out_sensor[${idx}].write(data_s);
                    flag_out_sensor[${idx}].write(flag_s);
                    selected_sensor = ${idx};
                    ready_sensor[selected_sensor].write(true);
                }
            }
            % endfor
            else{
                // Log an error if the address does not match any sensor
                fprintf(stderr,"\n[MESSY]\tAddress %d not found by functional bus\n", add_s);
                selected_sensor=-1;
                request_value.write(0x0);
                request_go.write(true);
            }
            // Write the index of the selected sensor to the output port
            idx_sensor.write(selected_sensor);
        }
        
        // Wait for the next event
        wait();

        if(selected_sensor>=0){
            // Check if a valid sensor was selected
            response();
            // Wait until the sensor and request are no longer ready
            while (go_sensors[selected_sensor].read() != false && request_ready.read() != false) {
                wait();
            }
            // Indicate that the request processing is complete
            request_go.write(false);
        }
 
    }
}    

/**
 * @brief Handles responses from sensors.
 * 
 * This function is responsible for handling the responses from the sensors
 * and sending them back to the master. It reads the data from the selected
 * sensor and writes it to the request_value output port. It also updates the
 * ready_sensor and request_go flags accordingly.
 */
void Functional_bus::response(){
    // Check if the selected sensor is ready
    if (go_sensors[selected_sensor].read() == true) {
        // Write the sensor data to the request_value output port
        request_value.write(data_input_sensor[selected_sensor].read());
        // Indicate that the response is ready
        request_go.write(true);
        // Reset the ready flag for the selected sensor
        ready_sensor[selected_sensor].write(false);
    }
}