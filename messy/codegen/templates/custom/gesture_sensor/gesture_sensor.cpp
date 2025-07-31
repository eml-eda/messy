#include <sensor_${sensor_name}_functional.hpp>

/**
 * @brief Main logic for the ${sensor_name} sensor.
 *
 * This function implements the core logic of the sensor, handling both read and
 * write operations. It also manages power signals based on the sensor's current state,
 * such as reading, writing, or idling. The hierarchy of the sensor logic is as follows:
 * - enable: Checks if the sensor is enabled. If true, proceeds to the next step, else idles (writes 0 to the power
 * signal).
 * - ready: This is the following step if the enable signal is true. If the sensor is also ready (ready signal is true),
 * the sensor proceeds to the next step, else writes false to the go signal.
 * - flag_wr: If the sensor is ready, it checks the flag_wr signal. If true, the sensor performs a read operation, else
 * a write operation.
 *
 * The sensor implements standard sensor behavior with:
 * - Control register (0x00): Start/stop bit to control sensor operation
 * - MODULE register (0x01): Sets the modulo value for data generation
 * - Status register (0x02): Indicates when new data is present
 * - Data register (0x03): Contains the sensor data, updated every 1 second with random value % MODULE
 *
 * The read and write operations are modeled using the following concept. In the configuration file, the user can
 * specify the delay and the power consumption for each operation. To mimic this behaviour, the sensor logic waits for
 * the specified delay time before proceeding to the next operation. During the read/write operation, the power signal
 * is set to the corresponding state (read/write) and then switched back to idle after the operation is complete. This
 * allows the power model to accurately simulate the power consumption of the sensor.
 */
void Sensor_${sensor_name}_functional::sensor_logic()
{
    while (true) {
        if (enable.read() == true) {
            if (ready.read() == true) {
                if (flag_wr.read() == true) {
                    // Read operation
#ifdef DEBUG_SENSOR_GESTURE
                    printf("[${sensor_name}] performing read operation at address 0x%x\n", address.read());
#endif
                    read_sensor(address.read());

                    // Reading power consumption state
                    power_signal.write(${sensor_name}_read); ///< Signal that the sensor is in reading state

                    // Request delay for the read operation
                    double start_time = sc_time_stamp().to_double(); ///< Get the current simulation time
                    core->request_delay(start_time, ${states["read"]["delay"]}, SIM_RESOLUTION);

                    // Switch to idle power consumption state
                    power_signal.write(${sensor_name}_idle);
                } else {
                    // Write operation
#ifdef DEBUG_SENSOR_GESTURE
                    printf("[${sensor_name}] performing write operation at address 0x%x\n", address.read());
#endif
                    write_sensor(address.read(), data_in.read(), req_size.read());

                    // Writing power consumption state
                    power_signal.write(${sensor_name}_write);
                    
                    // Request delay for the write operation
                    double start_time = sc_time_stamp().to_double(); ///< Get the current simulation time
                    core->request_delay(start_time, ${states["write"]["delay"]}, SIM_RESOLUTION); 

                    // Switch to idle power consumption state
                    power_signal.write(${sensor_name}_idle);
                }
                go.write(true); ///< Indicate that the operation is complete.
#ifdef DEBUG_SENSOR_GESTURE
                printf("[${sensor_name}] operation completed\n");
#endif
            } else {
                go.write(false);
            }

        } else {
            power_signal.write(0);
        }
        wait();
    }
}

void Sensor_${sensor_name}_functional::read_sensor(unsigned int address)
{
    // Handle specific register reads
    switch (address) {
    case CONTROL_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] reading CONTROL register: 0x%x\n", register_memory[CONTROL_REG_BASE]);
#endif
        data_out.write(register_memory + CONTROL_REG_BASE);
        break;
    case MODULE_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] reading MODULE register: 0x%x\n", register_memory[MODULE_REG_BASE]);
#endif
        data_out.write(register_memory + MODULE_REG_BASE);
        break;
    case STATUS_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] reading STATUS register: 0x%x\n", register_memory[STATUS_REG_BASE]);
#endif
        data_out.write(register_memory + STATUS_REG_BASE);
        break;
    case DATA_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] reading DATA register: 0x%x\n", register_memory[DATA_REG_BASE]);
#endif
        data_out.write(register_memory + DATA_REG_BASE);
        // Clear the new data status bit after reading
        register_memory[STATUS_REG_BASE] &= ~STATUS_NEW_DATA_BIT;
        break;
    default:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] reading generic register at address 0x%x: 0x%x\n", address, register_memory[address]);
#endif
        // For other addresses, send the pointer to the data in the register memory
        data_out.write(register_memory + address);
        break;
    }
}

void Sensor_${sensor_name}_functional::write_sensor(unsigned int address, uint8_t *data, unsigned int size)
{
#ifdef DEBUG_SENSOR_GESTURE
    printf("[${sensor_name}] write_sensor called for address 0x%x, size %u, data[0] = 0x%x\n", address, size, data[0]);
#endif
    // Handle specific register writes
    switch (address) {
    case CONTROL_REG_BASE:
        register_memory[CONTROL_REG_BASE] = data[0];
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] writing CONTROL register: 0x%x\n", data[0]);
#endif
        // Check if sensor should start or stop
        if (data[0] & CONTROL_START_BIT) {
            sensor_running = true;
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] sensor started\n");
#endif
        } else {
            sensor_running = false;
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] sensor stopped\n");
#endif
        }
        break;
    case MODULE_REG_BASE:
        register_memory[MODULE_REG_BASE] = data[0];
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] writing MODULE register: 0x%x\n", data[0]);
#endif
        // Ensure module_value is at least 1 to avoid division by zero
        if (register_memory[MODULE_REG_BASE] == 0) {
            register_memory[MODULE_REG_BASE] = 1;
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] MODULE register corrected to 1 to avoid division by zero\n");
#endif
        }
        break;
    case STATUS_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] attempted write to read-only STATUS register ignored\n");
#endif
        // Status register is read-only, ignore writes
        break;
    case DATA_REG_BASE:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] attempted write to read-only DATA register ignored\n");
#endif
        // Data register is read-only (updated by sensor), ignore writes
        break;
    default:
#ifdef DEBUG_SENSOR_GESTURE
        printf("[${sensor_name}] writing generic register at address 0x%x\n", address);
#endif
        // For other addresses, write normally
        for (unsigned int i = 0; i < size; i++) {
            register_memory[i + address] = data[i];
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] wrote 0x%x to address 0x%x\n", data[i], i + address);
#endif
        }
        break;
    }
    data_out.write(register_memory + address); ///< Send the pointer to the data in the register memory to the output
}

void Sensor_${sensor_name}_functional::data_update_thread()
{
    while (true) {
        wait(1, sc_core::SC_SEC); // Wait for the next time unit (1 second)


        // Update the data register if sensor is running
        if (sensor_running) {
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] updating data register\n");
            printf("[${sensor_name}] current MODULE value: %u\n", register_memory[MODULE_REG_BASE]);
#endif
            uint8_t new_value = rand() % register_memory[MODULE_REG_BASE];
            register_memory[DATA_REG_BASE] = new_value; ///< Update the data register with a random value
            // Set the new data present bit in status register
            register_memory[STATUS_REG_BASE] |= STATUS_NEW_DATA_BIT;
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] data updated: new value = 0x%x, STATUS = 0x%x\n", 
                   new_value, register_memory[STATUS_REG_BASE]);
#endif
        } else {
#ifdef DEBUG_SENSOR_GESTURE
            printf("[${sensor_name}] sensor not running, skipping data update\n");
#endif
        }
    }
}