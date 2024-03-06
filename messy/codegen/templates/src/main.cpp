#include <systemc.h>
#include <systemc-ams.h>
#include "core.h"
#include "core_power.h"
#include "core_converter.h"
#include "functional_bus.h"
#include "power_bus.h"
#include "load_converter.h"

% for sensor_name in peripherals["sensors"].keys():
#include "${sensor_name}_functional.h"
#include "${sensor_name}_power.h"
% endfor

% for harvester_name,harvester in peripherals["harvesters"].items():
#include "${harvester_name}.h"
% if "converter" in harvester:
#include "${harvester_name}_conv.h"
% endif
% endfor


int sc_main(int argc, char* argv[])
{
    Functional_bus                functional_bus("Func");
    Power_bus                     power_bus("Power");
    Core                          core("Master");
    Core_power                    core_power("Master_Power");
    core_power.core=&core;
    ${"core_converter" if "converter" in core else "Load_converter"} core_conv("Master Power Conv");

    sc_signal <int> core_state;
    //Master Power Signal
    sca_tdf::sca_signal <double> voltage_core;
    sca_tdf::sca_signal <double> current_core;
    sca_tdf::sca_signal <double> core_conv_current;
    //Functional Bus
    sc_signal <bool> enable_temp;
    //Data from Master to Functional Bus
    sc_signal <int>  core_request_address;
    sc_signal <int>  core_request_data;
    sc_signal <bool> core_request_ready;
    //Data from Functional Bus to Master
    sc_signal <int>  core_request_value;
    sc_signal <bool> core_request_go;
    //Data from Functional Bus to Slave
    sc_signal <int>  address_to_sensors[NUM_SENSORS];
    sc_signal <int>  data_to_sensors[NUM_SENSORS];
    sc_signal <bool> F_B_to_S[NUM_SENSORS];
    sc_signal <bool> ready_to_sensors[NUM_SENSORS];
    //Data from Slave to Functional Bus
    sc_signal <int> fake_data;
    sc_signal <bool> fake_go;
    functional_bus.data_input_sensor[0](fake_data);
    functional_bus.go_sensor[0](fake_go);

    //Power Bus
    //Signals from Slave to Bus
    sca_tdf::sca_signal <double> voltage_sensors[NUM_SENSORS];
    sca_tdf::sca_signal <double> current_sensors[NUM_SENSORS];
    //Output Traces from Power Bus
    sca_tdf::sca_signal <double> overall_voltage;

    % for harvester_name,harvester in peripherals["harvesters"].items():
    Harvester_${harvester_name} ${harvester_name}("${harvester_name}");
    ${f"{harvester_name}_converter" if "converter" in harvester else "Load_converter"} ${harvester_name}_conv("${harvester_name}_conv");
    % if harvester["harvester_type"]=="battery":
    sca_tdf::sca_signal <double> SoC_${harvester_name};
    ${harvester_name}.SoC(SoC_${harvester_name});
    % endif
    sca_tdf::sca_signal <double> voltage_${harvester_name};
    ${harvester_name}.voltage(voltage_${harvester_name});
    sca_tdf::sca_signal <double> current_${harvester_name};
    ${harvester_name}.current(current_${harvester_name});
    sca_tdf::sca_signal <double> current_${harvester_name}_bus;
    ${harvester_name}_conv.current_in(${f"current_{harvester_name}" if harvester["harvester_type"]=="battery" else f"current_{harvester_name}_bus"});
    ${harvester_name}_conv.voltage_in(voltage_${harvester_name});
    ${harvester_name}_conv.current_out(${f"current_{harvester_name}_bus" if harvester["harvester_type"]=="battery" else f"current_{harvester_name}"});
    % endfor

    % for idx,(sensor_name,sensor) in enumerate(peripherals["sensors"].items()):
    Sensor_${sensor_name}_functional ${sensor_name}("${sensor_name}");
    ${sensor_name}.core=&core;
    Sensor_${sensor_name}_power ${sensor_name}_power("${sensor_name}_power");
    // sensors don't support a real converter currently
    Load_converter ${sensor_name}_conv("${sensor_name}_converter");
    ${sensor_name}_conv.set_efficency(1.0);

    sc_signal <int> ${sensor_name}_Data;
    sc_signal <bool> ${sensor_name}_Go;  
    sc_signal <int> ${sensor_name}_F_to_P;
    sca_tdf::sca_signal <double> ${sensor_name}_V_State;
    sca_tdf::sca_signal <double> ${sensor_name}_I_State;
    sca_tdf::sca_signal <double> ${sensor_name}_I_S_to_C;

    ${sensor_name}.enable(enable_temp);
    ${sensor_name}.address(address_to_sensors[${idx}]);
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
    functional_bus.data_input_sensors[${idx}](${sensor_name}_Data);
    functional_bus.go_sensors[${idx}](${sensor_name}_Go);
    % endfor

    //Binding Functional Master's signals
    core.request_address(core_request_address);
    core.request_data(core_request_data);
    core.request_ready(core_request_ready);
    core.request_go(core_request_go);
    core.request_value(core_request_value);
    core.power_signal(core_state);
    //Binding Power Master's signals
    core_power.func_signal(core_state);
    core_power.voltage_state(voltage_core);
    core_power.current_state(current_core);
    core_conv.current_in(current_core);
    core_conv.voltage_in(voltage_core);
    core_conv.current_out(core_conv_current);
    //Binding Functional Bus Master's Input 
    functional_bus.request_address(core_request_address);
    functional_bus.request_data(core_request_data);
    functional_bus.request_ready(core_request_ready);
    //Binding Slave Input to Functional Bus

    //Binding Functional Bus's Output Address
    functional_bus.request_value(core_request_value);
    functional_bus.request_go(core_request_go);
    for (size_t i = 0; i < NUM_SENSORS; i++) {
        functional_bus.address_out_sensor[i](address_to_sensors[i]);
        functional_bus.data_out_sensor[i](data_to_sensors[i]);
        functional_bus.flag_out_sensor[i](F_B_to_S[i]);
        functional_bus.ready_sensor[i](ready_to_sensors[i]);
        power_bus.voltage_sensors[i](voltage_sensors[i]);
        power_bus.current_sensors[i](current_sensors[i]);
    }

    //Binding Power Bus's Input Signal
    power_bus.core_voltage(voltage_core);
    power_bus.core_current(core_conv_current);

    % for trace_type,trace_data in tracing.items():
    // define trace file
    sca_util::sca_trace_file* ${trace_type}_trace = sca_util::sca_create_tabular_trace_file("${trace_data['filename']}");
    % if "resolution" in trace_data:
    ${trace_type}_trace->set_mode(sca_sampling(${1 if "mult" not in trace_data["resolution"] else trace_data["resolution"]["mult"]},
    ${trace_data["resolution"]["unit"]}));
    % endif
    % endfor

    enable_temp.write(true);
    std::cout <<"The simulation starts!"<<endl;

    sc_start(SIM_LEN , SIM_RESOLUTION); // Set the simulation length

    std::cout <<"The simulation ends @ "<<sc_time_stamp()<<endl;

    core.close();

    //Close Trace Files
    % for trace_type,trace_data in tracing.items():
    sca_util::sca_close_tabular_trace_file(${trace_type}_trace);
    % endfor

    return 0;
}
