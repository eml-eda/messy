import argparse
from pathlib import Path

import json

import codegen_func


def main(input_file, template_dir, output_dir):
    # Load input settings file
    with open(input_file, "r") as f:
        settings = json.load(f)

    header_dir = output_dir / 'inc'
    header_dir.mkdir(parents=True, exist_ok=True)
    src_dir = output_dir / 'src'
    src_dir.mkdir(parents=True, exist_ok=True)

    #Battery selection statement 
    if settings["battery"] == "peukert" :
        codegen_func.battery_peukert_h_gen(template_dir, header_dir)
        codegen_func.battery_peukert_cpp_gen(template_dir,src_dir)
    elif settings["battery"] == "circuit":
        codegen_func.battery_circuit_h_gen(template_dir, header_dir)
        codegen_func.battery_circuit_cpp_gen(template_dir,src_dir)
    
    #Battery converter statement 

    # Generate Load_converter files
    codegen_func.load_converter_h_gen(template_dir, header_dir)
    codegen_func.load_converter_cpp_gen(template_dir, src_dir)

#    # Generate `X_sensor.h` file
#    codegen_func.sensor_h_gen(settings, template_dir, header_dir)
#    # Generate `X_sensor.cpp` file
#    codegen_func.sensor_cpp_gen(settings, template_dir, src_dir)
    
    #for sensor in settings['sensors']:
        #codegen_func.sensor_func_h_gen(sensor, template_dir, header_dir)
        #codegen_func.sensor_func_cpp_gen(sensor, template_dir, src_dir)
        #if sensor['power'] == True:
            #codegen_func.sensor_power_h_gen(sensor, template_dir, header_dir)
            #codegen_func.sensor_power_cpp_gen(sensor, template_dir, src_dir)
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
