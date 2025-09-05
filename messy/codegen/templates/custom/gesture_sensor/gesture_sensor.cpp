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
 * - Control register: to start/stop the sensor
 * - Status register: to indicate if new data is available
 * - Data register: to hold the latest sensor data
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
                double start_time = sc_time_stamp().to_double(); ///< Get the current simulation time
                if (flag_wr.read() == true) {
                    // Read operation
                    DEBUG_PRINT("[${sensor_name}] performing read operation at address 0x%x\n", address.read());
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
                    DEBUG_PRINT("[${sensor_name}] performing write operation at address 0x%x\n", address.read());
                    write_sensor(address.read(), data_in.read(), req_size.read());

                    // Writing power consumption state
                    power_signal.write(${sensor_name}_write);
                    
                    // Request delay for the write operation
                    core->request_delay(start_time, ${states["write"]["delay"]}, SIM_RESOLUTION); 

                    // Switch to idle power consumption state
                    power_signal.write(${sensor_name}_idle);
                }
                go.write(true); ///< Indicate that the operation is complete.
                DEBUG_PRINT("[${sensor_name}] operation completed\n");
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
        DEBUG_PRINT("[${sensor_name}] reading CONTROL register: 0x%x\n", register_memory[CONTROL_REG_BASE]);
        data_out.write(register_memory + CONTROL_REG_BASE);
        break;
    case MODULE_REG_BASE:
        DEBUG_PRINT("[${sensor_name}] reading MODULE register: 0x%x\n", register_memory[MODULE_REG_BASE]);
        data_out.write(register_memory + MODULE_REG_BASE);
        break;
    case STATUS_REG_BASE:
        DEBUG_PRINT("[${sensor_name}] reading STATUS register: 0x%x\n", register_memory[STATUS_REG_BASE]);
        data_out.write(register_memory + STATUS_REG_BASE);
        break;
    case DATA_REG_BASE:
        DEBUG_PRINT("[${sensor_name}] reading DATA register: 0x%x\n", register_memory[DATA_REG_BASE]);
        data_out.write(register_memory + DATA_REG_BASE);
        // Clear the new data status bit after reading
        register_memory[STATUS_REG_BASE] &= ~STATUS_NEW_DATA_BIT;
        break;
    default:
        DEBUG_PRINT("[${sensor_name}] reading generic register at address 0x%x: 0x%x\n", address, register_memory[address]);
        // For other addresses, send the pointer to the data in the register memory
        data_out.write(register_memory + address);
        break;
    }
}

void Sensor_${sensor_name}_functional::write_sensor(unsigned int address, uint8_t *data, unsigned int size)
{
    DEBUG_PRINT("[${sensor_name}] write_sensor called for address 0x%x, size %u\n", address, size);
    // Handle specific register writes
    switch (address) {
    case CONTROL_REG_BASE:
        // Copy all bytes to CONTROL register (up to CONTROL_REG_SIZE)
        for (unsigned int i = 0; i < size && i < CONTROL_REG_SIZE; i++) {
            register_memory[CONTROL_REG_BASE + i] = data[i];
            DEBUG_PRINT("[${sensor_name}] writing CONTROL register byte %u: 0x%x\n", i, data[i]);
        }
        // Check if sensor should start or stop (use first byte)
        if (data[0] & CONTROL_START_BIT) {
            sensor_running = true;
            DEBUG_PRINT("[${sensor_name}] sensor started\n");
        } else {
            sensor_running = false;
            DEBUG_PRINT("[${sensor_name}] sensor stopped\n");
        }
        break;
    case MODULE_REG_BASE:
        // Copy all bytes to MODULE register (up to MODULE_REG_SIZE)
        for (unsigned int i = 0; i < size && i < MODULE_REG_SIZE; i++) {
            register_memory[MODULE_REG_BASE + i] = data[i];
            DEBUG_PRINT("[${sensor_name}] writing MODULE register byte %u: 0x%x\n", i, data[i]);
        }
        // Ensure module_value is at least 1 to avoid division by zero (use first byte)
        if (register_memory[MODULE_REG_BASE] == 0) {
            register_memory[MODULE_REG_BASE] = 1;
            DEBUG_PRINT("[${sensor_name}] MODULE register corrected to 1 to avoid division by zero\n");
        }
        break;
    case STATUS_REG_BASE:
        DEBUG_PRINT("[${sensor_name}] attempted write to read-only STATUS register ignored\n");
        // Status register is read-only, ignore writes
        break;
    case DATA_REG_BASE:
        DEBUG_PRINT("[${sensor_name}] attempted write to read-only DATA register ignored\n");
        // Data register is read-only (updated by sensor), ignore writes
        break;
    default:
        DEBUG_PRINT("[${sensor_name}] writing generic register at address 0x%x\n", address);
        // For other addresses, write all bytes
        for (unsigned int i = 0; i < size; i++) {
            register_memory[i + address] = data[i];
            DEBUG_PRINT("[${sensor_name}] wrote 0x%x to address 0x%x\n", data[i], i + address);
        }
        break;
    }
    data_out.write(register_memory + address); ///< Send the pointer to the data in the register memory to the output
}

void Sensor_${sensor_name}_functional::data_update_thread()
{
    while (true) {
        // Update the data register if sensor is running
        if (sensor_running) {
            int bytes_read = read_next_sample();
            if (bytes_read > 0) {
                // Set the new data present bit in status register
                register_memory[STATUS_REG_BASE] |= STATUS_NEW_DATA_BIT;
                DEBUG_PRINT("[${sensor_name}] data updated: %d bytes, STATUS = 0x%x, sample = %ld\n", 
                    bytes_read, register_memory[STATUS_REG_BASE], dataset_current_sample);
            } else {
                DEBUG_PRINT("[${sensor_name}] no more samples, STATUS = 0x%x, sample = %ld\n", 
                    register_memory[STATUS_REG_BASE], dataset_current_sample);
            }
        }
        wait(DATASET_TIME_INTERVAL, DATASET_RESOLUTION);
    }
}

void Sensor_${sensor_name}_functional::open_dataset() {
    if (dataset_file) fclose(dataset_file);
    dataset_file = fopen(DATASET_PATH, "rb");
    if (!dataset_file) {
        printf("[${sensor_name}] ERROR: Could not open dataset file: %s\n", DATASET_PATH);
        sc_stop();
        return;
    }
    dataset_current_sample = 0;
    fseek(dataset_file, 0, SEEK_SET);
}

// Read the next sample (DATASET_SAMPLE_SIZE bytes) from the binary dataset into the data register
int Sensor_${sensor_name}_functional::read_next_sample() {
    if (!dataset_file) return 0;
    // Reads DATASET_SAMPLE_SIZE bytes into DATA register at once
    size_t bytes_read = fread(register_memory + DATA_REG_BASE, 1, DATASET_SAMPLE_SIZE, dataset_file);
    if (bytes_read < DATASET_SAMPLE_SIZE) {
        // EOF reached, cycle to first sample
        fseek(dataset_file, 0, SEEK_SET);
        bytes_read = fread(register_memory + DATA_REG_BASE, 1, DATASET_SAMPLE_SIZE, dataset_file);
        dataset_current_sample = 0;
    }
    if (bytes_read == DATASET_SAMPLE_SIZE) {
        dataset_current_sample++;
        return DATASET_SAMPLE_SIZE;
    }
    return 0;
}
