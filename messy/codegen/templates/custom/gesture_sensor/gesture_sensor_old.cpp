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

                // Compute Bluetooth delay
                int transmission_delay = compute_bt_delay(req_size.read());
                DEBUG_PRINT("[${sensor_name}] computed Bluetooth transmission delay: %d\n", transmission_delay);
                core->request_delay(start_time, transmission_delay, SIM_RESOLUTION);

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
    DEBUG_PRINT("[${sensor_name}] write_sensor called for address 0x%x, size %u, data[0] = 0x%x\n", address, size, data[0]);
    // Handle specific register writes
    switch (address) {
    case CONTROL_REG_BASE:
        register_memory[CONTROL_REG_BASE] = data[0];
        DEBUG_PRINT("[${sensor_name}] writing CONTROL register: 0x%x\n", data[0]);
        // Check if sensor should start or stop
        if (data[0] & CONTROL_START_BIT) {
            sensor_running = true;
            DEBUG_PRINT("[${sensor_name}] sensor started\n");
        } else {
            sensor_running = false;
            DEBUG_PRINT("[${sensor_name}] sensor stopped\n");
        }
        break;
    case MODULE_REG_BASE:
        register_memory[MODULE_REG_BASE] = data[0];
        DEBUG_PRINT("[${sensor_name}] writing MODULE register: 0x%x\n", data[0]);
        // Ensure module_value is at least 1 to avoid division by zero
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
        // For other addresses, write normally
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
            // Read the next line from the dataset
            uint8_t new_value = read_next_value();
            register_memory[DATA_REG_BASE] = new_value;
            // Set the new data present bit in status register
            register_memory[STATUS_REG_BASE] |= STATUS_NEW_DATA_BIT;
            DEBUG_PRINT("[${sensor_name}] data updated: new value = %d, STATUS = 0x%x, timestamp = %d\n", 
                new_value, register_memory[STATUS_REG_BASE], dataset_current_line);
        } else {
            DEBUG_PRINT("[${sensor_name}] sensor not running, skipping data update\n");
        }

        wait(DATASET_TIME_INTERVAL, DATASET_RESOLUTION);
    }
}

void Sensor_${sensor_name}_functional::open_dataset() {
    if (dataset_file) fclose(dataset_file);
    dataset_file = fopen(DATASET_PATH, "r");
    if (!dataset_file) {
        DEBUG_PRINT("[${sensor_name}] ERROR: Could not open dataset file: %s\n", DATASET_PATH);
        dataset_line_pos = 0;
        return;
    }
    // Skip header
    if (fgets(dataset_line_buf, sizeof(dataset_line_buf), dataset_file)) {
        dataset_line_pos = ftell(dataset_file);
    } else {
        dataset_line_pos = 0;
    }
}

uint8_t Sensor_${sensor_name}_functional::read_next_value() {
    if (!dataset_file) return 0;
    if (!fgets(dataset_line_buf, sizeof(dataset_line_buf), dataset_file)) {
        // EOF reached, cycle to first data line
        dataset_current_line = 0;
        fseek(dataset_file, dataset_line_pos, SEEK_SET);
        if (!fgets(dataset_line_buf, sizeof(dataset_line_buf), dataset_file)) return 0;
    }
    char *comma = strchr(dataset_line_buf, ',');
    if (comma) {
        dataset_current_line++;
        return (uint8_t)atoi(comma + 1);
    }
    return 0;
}

#define UNIFORM_OPEN01() ((double)rand() / (double)RAND_MAX) // random in [0,1]

// Computes the Bluetooth transmission delay given the request size in bytes
int Sensor_${sensor_name}_functional::compute_bt_delay(unsigned int req_bytes) {
    // TODO: sanitize parameters
    // Bluetooth parameters from configuration
    double bt_conn_int = ${states["bluetooth"]["connection_interval"]};
    double bt_event = ${states["bluetooth"]["connection_event"]};
    double bt_jitter = ${states["bluetooth"]["jitter"]};
    double bt_drop = ${states["bluetooth"]["fragment_drop_chance"]};
    double bt_frag_size = ${states["bluetooth"]["fragment_size_bytes"]};

    // Number of fragments is ceil(req_bytes / bt_frag_size)
    unsigned int n_frags = (req_bytes + (unsigned int)bt_frag_size - 1) / (unsigned int)bt_frag_size; // ceil

    // Jitter is a random value in [-bt_jitter, +bt_jitter]
    double jitter_val = UNIFORM_OPEN01() * 2.0 * bt_jitter - bt_jitter;

    // Number of trials based on bt_drop probability of each fragment
    unsigned int trials = 0;
    for (unsigned int i = 0; i < n_frags; ++i) {
        ++trials; // Add the initial trial
        double p = 1 - bt_drop;
        while (UNIFORM_OPEN01() > p) ++trials; // Geometric distribution
    }

    // Total bluetooth delay
    int transmission_delay = (int)((bt_conn_int * trials) + bt_event + jitter_val);
    DEBUG_PRINT("[${sensor_name}] [compute_bt_delay] frags=%u, trials=%u, jitter=%.2f, delay=%d\n", n_frags, trials, jitter_val, transmission_delay);
    return transmission_delay;
}