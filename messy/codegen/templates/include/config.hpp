#ifndef __CONFIG__H__
#define __CONFIG__H__

/**
 * @brief Reference voltage for the bus.
 */
#define VREF_BUS ${bus["vref"]}

/**
 * @brief Number of sensors available in the system.
 */
#define NUM_SENSORS ${len(peripherals["sensors"])}

/**
 * @brief Number of sources in the harvester peripherals.
 */
#define NUM_SOURCES ${len([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="source"])}

/**
 * @brief Number of batteries in the harvester peripherals.
 */
#define NUM_BATTERIES ${len([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="battery"])}

/**
 * @brief Total number of harvesters in the system.
 */
#define NUM_HARVESTERS ${len(peripherals["harvesters"])}

/**
 * @brief Resolution of the simulation.
 * 
 * The resolution of the simulation should be compliant with what SystemC can handle. The allowed values are:
 * 
 * - SC_SEC: 1s
 * - SC_MS: 1ms
 * 
 * The preferred value is SC_MS, as it provides a good trade-off between simulation time and accuracy. 
 */
#define SIM_RESOLUTION ${resolution}

/**
 * @brief Length of the simulation. The measurement unit is defined by SIM_RESOLUTION.
 */
#define SIM_LEN 1000000

% for sensor_name,sensor in peripherals["sensors"].items():

/**
 * @brief Base address for the ${sensor_name} sensor.
 */
#define ${sensor_name}_BASE_ADDRESS ${sensor["base"]}

/**
 * @brief Enum defining the states for the ${sensor_name} sensor.
 */
typedef enum {
    ${sensor_name}_OFF__,
    % for s_state_name in sensor["states"]:
    ${sensor_name}_${s_state_name},
    % endfor 
} ${sensor_name}_states;
% endfor

#endif