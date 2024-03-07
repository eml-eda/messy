#ifndef __CONFIG__H__
#define __CONFIG__H__

#define VREF_BUS ${bus["vref"]}


#define NUM_SENSORS ${len(peripherals["sensors"])}
#define NUM_HARVESTERS ${len(peripherals["harvesters"])}

#define SIM_RESOLUTION ${resolution}

#define SIM_LEN 1000000

% for sensor_name,sensor in peripherals["sensors"].items():
#define ${sensor_name}_BASE_ADDRESS ${sensor["base"]}
typedef enum{
    ${sensor_name}_OFF__
    % for s_state_name in sensor["states"]:
    ,${sensor_name}_${s_state_name}
    % endfor 
}${sensor_name}_states;
% endfor

#endif