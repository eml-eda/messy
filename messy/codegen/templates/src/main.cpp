#include <systemc.h>
#include <systemc-ams.h>
#include <core.hpp>
#include <core_power.hpp>
% if "converter" in core:
#include <converter/core_converter.hpp>
% endif
#include <functional_bus.hpp>
#include <power_bus.hpp>
#include <converter/load_converter.hpp>

% for sensor_name in peripherals["sensors"].keys():
#include <sensor_${sensor_name}_functional.hpp>
#include <sensor_${sensor_name}_power.hpp>
% endfor

% for harvester_name,harvester in peripherals["harvesters"].items():
#include <harvester_${harvester_name}.hpp>
% if "converter" in harvester:
#include <converter/${harvester_name}_converter.hpp>
% endif
% endfor


int sc_main(int argc, char* argv[])
{
    Functional_bus                functional_bus("Func");  /**< Functional bus module */
    Power_bus                     power_bus("Power"); /**< Power bus module */
    Core                          core("Master"); /**< Core module */
    Core_power                    core_power("Master_Power"); /**< Core power module */
    core_power.core=&core;
    ${"core_converter" if "converter" in core else "Load_converter"} core_conv("Master_Power_Conv"); /**< Core converter module */

    sc_signal <double> core_state; /**< Signal for the core's functional state. */

    // Master (Core) Power Signals
    sca_tdf::sca_signal <double> voltage_core; /**< Voltage signal for the core. */
    sca_tdf::sca_signal <double> current_core; /**< Current signal for the core. */
    sca_tdf::sca_signal <double> core_conv_current; /**< Converted current signal for the core. */
    
    /********************************************
     * Functional Bus Signals
     ********************************************/

    sc_signal <bool> enable_temp; /**< Enable signal for the functional bus. */

    // Data from Master (Core) to Functional Bus
    sc_signal <unsigned int>  core_request_address; /**< Address of the request from the core. */
    sc_signal <uint8_t*>  core_request_data; /**< Data of the request from the core. */
    sc_signal <unsigned int>  core_request_size; /**< Size of the request from the core. */
    sc_signal <bool> core_request_ready; /**< Flag indicating if the core's request is ready. */
    sc_signal <bool> core_functional_bus_flag; /**< Flag for the functional bus from the core. */
    
    // Data from Functional Bus to Master
    sc_signal <uint8_t*>  core_request_value; /**< Value of the request from the functional bus to the core. */
    sc_signal <bool> core_request_go; /**< Flag indicating if the functional bus is ready to process the core's request. */
    sc_signal <int> idx_sensor; /**< Index of the selected sensor for the core's request. */
    
    // Data from Functional Bus to Slave (Sensors)
    sc_signal <unsigned int>  address_to_sensors[NUM_SENSORS]; /**< Array of addresses to sensors. */
    sc_signal <unsigned int>  size_to_sensors[NUM_SENSORS]; /**< Array of sizes to sensors. */
    sc_signal <uint8_t*>  data_to_sensors[NUM_SENSORS]; /**< Array of data to sensors. */
    sc_signal <bool> F_B_to_S[NUM_SENSORS]; /**< Array of functional bus to sensor flags. */
    sc_signal <bool> ready_to_sensors[NUM_SENSORS]; /**< Array of ready flags to sensors. */

    /********************************************
     * Power Bus Signals
     ********************************************/

    // Signals from Slave (Sensors) to Power Bus
    sca_tdf::sca_signal <double> voltage_sensors[NUM_SENSORS]; /**< Array of voltage signals from sensors to the power bus. */
    sca_tdf::sca_signal <double> current_sensors[NUM_SENSORS]; /**< Array of current signals from sensors to the power bus. */
#if NUM_SOURCES>0
    sca_tdf::sca_signal <double> current_sources[NUM_SOURCES]; /**< Array of current signals from sources to the power bus. */
#endif
#if NUM_BATTERIES>0
    sca_tdf::sca_signal <double> current_batteries[NUM_BATTERIES]; /**< Array of current signals from batteries to the power bus. */
#endif

    // Output Traces from Power Bus
    sca_tdf::sca_signal <double> overall_voltage; /**< Overall voltage signal from the power bus. */

    % for harvester_name,harvester in peripherals["harvesters"].items():
    /********************************************
     * ${harvester_name} Module
     ********************************************/
    Harvester_${harvester_name} ${harvester_name}("${harvester_name}"); /**< Harvester module instance. */
    ${f"{harvester_name}_converter" if "converter" in harvester else "Load_converter"} ${harvester_name}_conv("${harvester_name}_conv"); /**< Harvester converter module instance. */
    % if harvester["harvester_type"]=="battery":
    sca_tdf::sca_signal <double> SoC_${harvester_name}; /**< State of Charge signal for the ${harvester_name} battery. */
    ${harvester_name}.soc(SoC_${harvester_name});
    % endif
    sca_tdf::sca_signal <double> voltage_${harvester_name}; /**< Voltage signal for the ${harvester_name}. */
    sca_tdf::sca_signal <double> current_${harvester_name}; /**< Current signal for the ${harvester_name}. */
    sca_tdf::sca_signal <double> current_${harvester_name}_bus; /**< Bus current signal for the ${harvester_name}. */
    ${harvester_name}.v(voltage_${harvester_name});
    ${harvester_name}.i(current_${harvester_name}_bus);

    // Converter binding for the ${harvester_name}
    ${harvester_name}_conv.current_in(${f"current_{harvester_name}" if harvester["harvester_type"]=="battery" else f"current_{harvester_name}_bus"});
    ${harvester_name}_conv.voltage_in(voltage_${harvester_name});
    ${harvester_name}_conv.current_out(${f"current_{harvester_name}_bus" if harvester["harvester_type"]=="battery" else f"current_{harvester_name}"});
    % endfor

    % for idx,(sensor_name,sensor) in enumerate(peripherals["sensors"].items()):
    /********************************************
     * ${sensor_name} Module
     ********************************************/
    Sensor_${sensor_name}_functional ${sensor_name}("${sensor_name}"); /**< Functional sensor module instance. */
    ${sensor_name}.core=&core;
    Sensor_${sensor_name}_power ${sensor_name}_power("${sensor_name}_power"); /**< Power sensor module instance. */
    // sensors don't support a real converter currently
    Load_converter ${sensor_name}_conv("${sensor_name}_converter"); /**< Sensor converter module instance. */
    ${sensor_name}_conv.set_efficency(1.0);

    sc_signal <uint8_t*> ${sensor_name}_Data; /**< Data signal for ${sensor_name}. */
    sc_signal <bool> ${sensor_name}_Go;  /**< Go signal for ${sensor_name}. */
    sc_signal <int> ${sensor_name}_F_to_P; /**< Functional to Power signal for ${sensor_name}. */
    sca_tdf::sca_signal <double> ${sensor_name}_V_State; /**< Voltage state signal for ${sensor_name}. */
    sca_tdf::sca_signal <double> ${sensor_name}_I_State; /**< Current state signal for ${sensor_name}. */
    sca_tdf::sca_signal <double> ${sensor_name}_I_S_to_C; /**< Current signal from ${sensor_name} to converter. */

    // Binding ${sensor_name} signals to the power instance, functional bus and converter
    ${sensor_name}.enable(enable_temp);
    ${sensor_name}.address(address_to_sensors[${idx}]);
    ${sensor_name}.req_size(size_to_sensors[${idx}]);
    ${sensor_name}.data_in(data_to_sensors[${idx}]);
    ${sensor_name}.flag_wr(F_B_to_S[${idx}]);
    ${sensor_name}.ready(ready_to_sensors[${idx}]);
    ${sensor_name}.data_out(${sensor_name}_Data);
    ${sensor_name}.go(${sensor_name}_Go);
    ${sensor_name}.power_signal(${sensor_name}_F_to_P);
    ${sensor_name}_power.func_signal(${sensor_name}_F_to_P);
    ${sensor_name}_power.voltage_state(voltage_sensors[${idx}]);
    ${sensor_name}_power.current_state(${sensor_name}_I_S_to_C);
    ${sensor_name}_conv.current_in(${sensor_name}_I_S_to_C);
    ${sensor_name}_conv.voltage_in(voltage_sensors[${idx}]);
    ${sensor_name}_conv.current_out(current_sensors[${idx}]);
    functional_bus.data_input_sensor[${idx}](${sensor_name}_Data);
    functional_bus.go_sensors[${idx}](${sensor_name}_Go);
    % endfor

    // Binding Functional Master's signals
    core.request_address(core_request_address);
    core.request_data(core_request_data);
    core.request_size(core_request_size);
    core.request_ready(core_request_ready);
    core.request_go(core_request_go);
    core.request_value(core_request_value);
    core.idx_sensor(idx_sensor);
    core.functional_bus_flag(core_functional_bus_flag);
    core.power_signal(core_state);

    // Binding Power Master's signals
    core_power.func_signal(core_state);
    core_power.voltage_state(voltage_core);
    core_power.current_state(current_core);
    core_conv.current_in(current_core);
    core_conv.voltage_in(voltage_core);
    core_conv.current_out(core_conv_current);

    // Binding Functional Bus Master's Input 
    functional_bus.request_address(core_request_address);
    functional_bus.request_data(core_request_data);
    functional_bus.request_ready(core_request_ready);
    functional_bus.request_size(core_request_size);
    functional_bus.idx_sensor(idx_sensor);
    functional_bus.flag_from_core(core_functional_bus_flag);

    // Binding Functional Bus's Output Address
    functional_bus.request_value(core_request_value);
    functional_bus.request_go(core_request_go);
    for (size_t i = 0; i < NUM_SENSORS; i++) {
        functional_bus.address_out_sensor[i](address_to_sensors[i]);
        functional_bus.data_out_sensor[i](data_to_sensors[i]);
        functional_bus.flag_out_sensor[i](F_B_to_S[i]);
        functional_bus.size_out_sensor[i](size_to_sensors[i]);
        functional_bus.ready_sensor[i](ready_to_sensors[i]);
        power_bus.voltage_sensors[i](voltage_sensors[i]);
        power_bus.current_sensors[i](current_sensors[i]);
    }

    // Binding Battery and Source Harvesters
    % for idx,batt_name in enumerate([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="battery"]):
    power_bus.current_batteries[${idx}](current_${batt_name});
    % endfor
    % for idx,source_name in enumerate([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="source"]):
    power_bus.current_sources[${idx}](current_${source_name});
    % endfor

    // Binding Power Bus's Input Signals related to the Master (Core)
    power_bus.core_voltage(voltage_core);
    power_bus.core_current(core_conv_current);

    % for trace_name,trace_data in tracing.items():

    // define trace file
    sca_util::sca_trace_file* ${trace_name} = sca_util::sca_create_tabular_trace_file("${trace_data['filename']}");
    % if "resolution" in trace_data:
    ${trace_name}->set_mode(sca_sampling(${1 if "mult" not in trace_data["resolution"] else trace_data["resolution"]["mult"]},${trace_data["resolution"]["unit"]}));
    % endif

    // define the signals to trace
    % for traces_obj in trace_data["traces"]:
    % for log_name,log_signal in utils["get_signals"](traces_obj).items():
    sca_util::sca_trace(${trace_name}, ${log_signal}, "${log_name}");
    % endfor
    % endfor
    % endfor

    // Start the simulation
    enable_temp.write(true);
    std::cout <<"The simulation starts!"<<endl;

    sc_start(SIM_LEN , SIM_RESOLUTION); // Set the simulation length

    std::cout <<"The simulation ends @ "<<sc_time_stamp()<<endl;

    // Close Trace Files
    % for trace_name,trace_data in tracing.items():
    sca_util::sca_close_tabular_trace_file(${trace_name});
    % endfor

    return 0;
}
