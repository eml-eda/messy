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
% for sensor_name, sensor in peripherals['sensors'].items():
    sc_signal <unsigned int>  address_to_${sensor_name}; /**< Address of the request to the ${sensor_name} sensor. */
    sc_signal <unsigned int>  size_to_${sensor_name}; /**< Size of the request to the ${sensor_name} sensor. */
    sc_signal <uint8_t*>  data_to_${sensor_name}; /**< Data of the request to the ${sensor_name} sensor. */
    sc_signal <bool> F_B_to_${sensor_name}; /**< Flag indicating if the request to the ${sensor_name} sensor is a read operation. */
    sc_signal <bool> ready_to_${sensor_name}; /**< Flag indicating if the ${sensor_name} sensor is ready to process the request. */
    sca_tdf::sca_signal <double> voltage_${sensor_name}; /**< Voltage signal for the ${sensor_name} sensor. */
    sca_tdf::sca_signal <double> current_${sensor_name}; /**< Current signal for the ${sensor_name} sensor. */
% endfor

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
    ${sensor_name}.i_is_enabled(enable_temp);
    ${sensor_name}.i_address(address_to_${sensor_name});
    ${sensor_name}.i_size(size_to_${sensor_name});
    ${sensor_name}.i_data_ptr(data_to_${sensor_name});
    ${sensor_name}.i_is_read(F_B_to_${sensor_name});
    ${sensor_name}.i_is_active(ready_to_${sensor_name});
    ${sensor_name}.o_data_ptr(${sensor_name}_Data);
    ${sensor_name}.o_is_done(${sensor_name}_Go);
    ${sensor_name}.o_power_state(${sensor_name}_F_to_P);
    ${sensor_name}_power.i_power_state(${sensor_name}_F_to_P);
    ${sensor_name}_power.o_voltage_a(voltage_${sensor_name});
    ${sensor_name}_power.o_current_a(${sensor_name}_I_S_to_C);
    ${sensor_name}_conv.current_in(${sensor_name}_I_S_to_C);
    ${sensor_name}_conv.voltage_in(voltage_${sensor_name});
    ${sensor_name}_conv.current_out(current_${sensor_name});
    functional_bus.i_data_${sensor_name}_ptr(${sensor_name}_Data);
    functional_bus.i_is_done_${sensor_name}(${sensor_name}_Go);
    % endfor

    // Binding Functional Master's signals
    core.o_address(core_request_address);
    core.o_data_ptr(core_request_data);
    core.o_size(core_request_size);
    core.o_activate_functional_bus(core_request_ready);
    core.i_done_functional_bus(core_request_go);
    core.i_data_ptr(core_request_value);
    core.i_idx_sensor(idx_sensor);
    core.o_is_read(core_functional_bus_flag);
    core.o_power_state(core_state);

    // Binding Power Master's signals
    core_power.i_power_state(core_state);
    core_power.o_voltage_a(voltage_core);
    core_power.o_current_a(current_core);
    core_conv.current_in(current_core);
    core_conv.voltage_in(voltage_core);
    core_conv.current_out(core_conv_current);

    // Binding Functional Bus Master's Input 
    functional_bus.i_address(core_request_address);
    functional_bus.i_data_ptr(core_request_data);
    functional_bus.i_is_active(core_request_ready);
    functional_bus.i_size(core_request_size);
    functional_bus.o_idx_sensor(idx_sensor);
    functional_bus.i_is_read(core_functional_bus_flag);

    // Binding Functional Bus's Output Address
    functional_bus.o_data_ptr(core_request_value);
    functional_bus.o_is_done(core_request_go);

% for idx, (sensor_name, sensor) in enumerate(peripherals["sensors"].items()):
    functional_bus.o_address_${sensor_name}(address_to_${sensor_name});
    functional_bus.o_data_${sensor_name}_ptr(data_to_${sensor_name});
    functional_bus.o_is_read_${sensor_name}(F_B_to_${sensor_name});
    functional_bus.o_size_${sensor_name}(size_to_${sensor_name});
    functional_bus.o_activate_${sensor_name}(ready_to_${sensor_name});
    // Binding Power Bus's Input Signals related to the ${sensor_name} sensor
    power_bus.i_voltage_${sensor_name}_a(voltage_${sensor_name});
    power_bus.i_current_${sensor_name}_a(current_${sensor_name});
% endfor

    // Binding Battery and Source Harvesters
    % for idx,batt_name in enumerate([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="battery"]):
    power_bus.i_current_batteries_a[${idx}](current_${batt_name});
    % endfor
    % for idx,source_name in enumerate([harv_name for harv_name,harv in peripherals["harvesters"].items() if harv["harvester_type"]=="source"]):
    power_bus.i_current_sources_a[${idx}](current_${source_name});
    % endfor

    // Binding Power Bus's Input Signals related to the Master (Core)
    power_bus.i_voltage_core_a(voltage_core);
    power_bus.i_current_core_a(core_conv_current);

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
