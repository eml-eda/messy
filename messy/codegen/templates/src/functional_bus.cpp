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
        if (i_is_active.read() == true) {
            // Read the request details from the input ports
            add_s = i_address.read();
            flag_s = i_is_read.read();
            size_s = i_size.read();

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
                    _selected_sensor=-1;
                    o_data_ptr.write(0x0);
                    o_is_done.write(true);
                }
                else{
                    // Calculate the register address within the sensor
                    reg_s = add_s - ${sensor_name}_BASE_ADDRESS;
                    data_s = i_data_ptr.read();
                    // Set the output signals to the sensor
                    o_size[${idx}].write(size_s);
                    o_address[${idx}].write(reg_s);
                    o_data_sensors_ptr[${idx}].write(data_s);
                    o_is_read[${idx}].write(flag_s);
                    _selected_sensor = ${idx};
                    o_activate_${sensor_name}.write(true);
                }
            }
            % endfor
            else{
                // Log an error if the address does not match any sensor
                fprintf(stderr,"\n[MESSY]\tAddress %d not found by functional bus\n", add_s);
                _selected_sensor=-1;
                o_data_ptr.write(0x0);
                o_is_done.write(true);
            }
            // Write the index of the selected sensor to the output port
            o_idx_sensor.write(_selected_sensor);
        }
        
        // Wait for the next event
        wait();

        if(_selected_sensor>=0){
            // Check if a valid sensor was selected
            response();
            % for idx, (sensor_name, sensor) in enumerate(peripherals['sensors'].items()):
            if (_selected_sensor == ${idx}) {
                // Check if the selected sensor is done
                while (i_is_done_${sensor_name}.read() == true && i_is_active.read() == true) {
                    wait();
                }
            }
            % endfor
            // Indicate that the request processing is complete
            o_is_done.write(false);
        }
 
    }
}    

/**
 * @brief Handles responses from sensors.
 * 
 * This function is responsible for handling the responses from the sensors
 * and sending them back to the master. It reads the data from the selected
 * sensor and writes it to the o_data_ptr output port. It also updates the
 * o_activate_sensors and o_is_done flags accordingly.
 */
void Functional_bus::response(){
% for sensor_name, sensor in peripherals['sensors'].items():
    if (i_is_done_${sensor_name}.read() == true) {
        // Write the sensor data to the o_data_ptr output port
        o_data_ptr.write(i_data_${sensor_name}_ptr.read());
        // Indicate that the response is ready
        o_is_done.write(true);
        // Reset the ready flag for the selected sensor
        o_activate_${sensor_name}.write(false);
    }
% endfor
}