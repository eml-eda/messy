#include <sensor_${sensor_name}_functional.hpp>

/**
 * @brief Main logic for the ${sensor_name} sensor.
 * 
 * This function implements the core logic of the sensor, handling both read and
 * write operations. It also manages power signals based on the sensor's current state,
 * such as reading, writing, or idling. The hierarchy of the sensor logic is as follows:
 * - enable: Checks if the sensor is enabled. If true, proceeds to the next step, else idles (writes 0 to the power signal).
 * - ready: This is the following step if the enable signal is true. If the sensor is also ready (ready signal is true), the sensor proceeds to the next step, else writes false to the go signal. 
 * - flag_wr: If the sensor is ready, it checks the flag_wr signal. If true, the sensor performs a read operation, else a write operation. 
 * 
 * 
 * The read and write operations are modeled using the following concept. In the configuration file, the user can specify the delay and the power consumption for each operation. To mimic this behaviour, the sensor logic waits for the specified delay time before proceeding to the next operation. During the read/write operation, the power signal is set to the corresponding state (read/write) and then switched back to idle after the operation is complete. This allows the power model to accurately simulate the power consumption of the sensor.
 */
void Sensor_${sensor_name}_functional::sensor_logic() {
    % if memory_init:
    % for idx,val in enumerate(memory_init):
    register_memory[idx]=val;
    % endfor 
    % endif 
    while (true) {
        if (enable.read() == true) {
            if (ready.read() == true) {
                if (flag_wr.read() == true) {
                    // Read operation
                    unsigned int add = address.read(); ///< Get the address from the input

                    // Perform read operation
                    data_out.write(register_memory + add); ///< Send the pointer to the data in the register memory to the output.
                    
                    // Reading power consumption state
                    power_signal.write(${sensor_name}_read); ///< Signal that the sensor is in reading state

                    // Request delay for the read operation
                    double start_time = sc_time_stamp().to_double(); ///< Get the current simulation time
                    core->request_delay(start_time, ${states["read"]["delay"]}, SIM_RESOLUTION);

                    // Switch to idle power consumption state
                    power_signal.write(${sensor_name}_idle);
                    go.write(true); ///< Indicate that the operation is complete.
                } else {
                    // Write operation
                    unsigned int add = address.read(); ///< Get the address from the input

                    // Prepare address and data for writing
                    unsigned int req_size_val = req_size.read(); ///< Get the size of the write request
                    uint8_t *req_core_val_addr = data_in.read(); ///< Get the data to be written to the register memory

                    // Write data to the register memory
                    for (unsigned int i = 0; i < req_size_val; i++)
                        register_memory[i + add] = req_core_val_addr[i];

                    data_out.write(register_memory + add); ///< Send the pointer to the data in the register memory to the output

                    // Writing power consumption state
                    power_signal.write(${sensor_name}_write);
                    
                    // Request delay for the write operation
                    double start_time = sc_time_stamp().to_double(); ///< Get the current simulation time
                    core->request_delay(start_time, ${states["write"]["delay"]}, SIM_RESOLUTION); 

                    // Switch to idle power consumption state
                    power_signal.write(${sensor_name}_idle);
                    go.write(true); ///< Indicate that the operation is complete.
                }
            } else {
                go.write(false);
            }

        } else {
            power_signal.write(0);
        }
        wait();
    }
}
