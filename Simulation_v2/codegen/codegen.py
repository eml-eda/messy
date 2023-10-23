import argparse
from pathlib import Path

import json

import codegen_func


def main(input_file, template_dir, output_dir):
    # Load input settings file
    with open(input_file, "r") as f:
        settings = json.load(f)

    # Adding internal variable
    i = 1
    baseaddress = 0

    for sensor in settings["sensors"]:
        sensor["base"] = baseaddress
        sensor["ID"] = i
        sensor["reg_name"] = sensor["name"].upper() + "_REG_DIMENSION"
        sensor["v_name"] = sensor["name"].upper() + "_VOLTAGE"
        baseaddress = baseaddress + 1 + sensor["reg"]
        i += 1

    header_dir = output_dir / 'inc'
    header_dir.mkdir(parents=True, exist_ok=True)
    src_dir = output_dir / 'src'
    src_dir.mkdir(parents=True, exist_ok=True)

    #Config.h statement
    codegen_func.config_gen(settings, template_dir, header_dir)

    #Functional Bus statement
    codegen_func.sys_functional_bus_h_gen(template_dir,header_dir)
    codegen_func.sys_functional_bus_cpp_gen(settings,template_dir,src_dir)

    #Power Bus statement
    codegen_func.sys_power_bus_h_gen(template_dir,header_dir)
    codegen_func.sys_power_bus_cpp_gen(template_dir,src_dir)

    #Battery selection statement 
    if settings["battery"] == "peukert":
        codegen_func.battery_peukert_h_gen(template_dir, header_dir)
        codegen_func.battery_peukert_cpp_gen(template_dir,src_dir)
    elif settings["battery"] == "circuit":
        codegen_func.battery_circuit_h_gen(template_dir, header_dir)
        codegen_func.battery_circuit_cpp_gen(template_dir,src_dir)
    
    #Battery converter statement 
    codegen_func.battery_converter_h_gen(template_dir, header_dir)
    codegen_func.battery_converter_cpp_gen(template_dir, src_dir)

    #PV panel statement
    codegen_func.pv_panel_h_gen(template_dir, header_dir)
    codegen_func.pv_panel_cpp_gen(template_dir, src_dir)

    #PV panel converter statement
    codegen_func.converter_pv_h_gen(template_dir, header_dir)
    codegen_func.converter_pv_cpp_gen(template_dir, src_dir)

    #Misc statement
    codegen_func.lut_h_gen(template_dir, header_dir)
    codegen_func.lut_cpp_gen(template_dir, src_dir)

    #Sensor statement
    for sensor in settings['sensors']:
        codegen_func.sensor_func_h_gen(sensor, template_dir, header_dir)
        codegen_func.sensor_func_cpp_gen(sensor, template_dir, src_dir)
        if sensor['power'] == True:
            codegen_func.sensor_power_h_gen(sensor, template_dir, header_dir)
            codegen_func.sensor_power_cpp_gen(sensor, template_dir, src_dir)

    #Load converter statement
    codegen_func.load_converter_h_gen(template_dir, header_dir)
    codegen_func.load_converter_cpp_gen(template_dir, src_dir)

    #Core statement 
    codegen_func.core_h_gen(template_dir, header_dir)
    codegen_func.core_cpp_gen(settings,template_dir, src_dir)
    codegen_func.core_power_h_gen(template_dir, header_dir)
    codegen_func.core_power_cpp_gen(template_dir, src_dir)

    #Main statement
    codegen_func.main_cpp_gen(settings,template_dir, src_dir)

    print ("OK!")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate src code')
    parser.add_argument('-f', '--input-file', type=str, metavar='PATH',
                        help='Path to JSON input simulation settings')
    parser.add_argument('-t', '--template-dir', type=str, metavar='PATH',
                        default='./templates',
                        help='Path to templates directory')
    parser.add_argument('-o', '--output-dir', type=str, metavar='PATH',
                        default='.',
                        help='Path where code will be generated')
    args = parser.parse_args()

    input_file = Path(args.input_file)
    template_dir = Path(args.template_dir)
    output_dir = Path(args.output_dir)

    main(input_file, template_dir, output_dir)
