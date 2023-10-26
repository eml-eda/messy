import argparse
from pathlib import Path

import json

import codegen_func


def main(input_file, template_dir, output_dir):
    """
    Main function for code generation

    Parameters
    ----------
    input_file : Path
        Path to JSON input simulation settings
    template_dir : Path
        Path to templates directory
    output_dir : Path
        Path where code will be generated
    """

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

    header_dir = output_dir / "inc"
    header_dir.mkdir(parents=True, exist_ok=True)
    src_dir = output_dir / "src"
    src_dir.mkdir(parents=True, exist_ok=True)

    # Config.h statement
    codegen_func.config_gen(settings, template_dir, header_dir)

    # Functional Bus statement
    codegen_func.sys_functional_bus_h_gen(template_dir, header_dir)
    codegen_func.sys_functional_bus_cpp_gen(settings, template_dir, src_dir)

    # Power Bus statement
    codegen_func.sys_power_bus_h_gen(template_dir, header_dir)
    codegen_func.sys_power_bus_cpp_gen(template_dir, src_dir)

    # Battery selection statement
    if settings["battery"] == "peukert":
        codegen_func.battery_peukert_h_gen(template_dir, header_dir)
        codegen_func.battery_peukert_cpp_gen(template_dir, src_dir)
    elif settings["battery"] == "circuit":
        codegen_func.battery_circuit_h_gen(template_dir, header_dir)
        codegen_func.battery_circuit_cpp_gen(template_dir, src_dir)

    # Battery converter statement
    codegen_func.battery_converter_h_gen(template_dir, header_dir)
    codegen_func.battery_converter_cpp_gen(template_dir, src_dir)

    # PV panel statement
    codegen_func.pv_panel_h_gen(template_dir, header_dir)
    codegen_func.pv_panel_cpp_gen(template_dir, src_dir)

    # PV panel converter statement
    codegen_func.converter_pv_h_gen(template_dir, header_dir)
    codegen_func.converter_pv_cpp_gen(template_dir, src_dir)

    # Misc statement
    codegen_func.lut_h_gen(template_dir, header_dir)
    codegen_func.lut_cpp_gen(template_dir, src_dir)

    # Sensor statement
    for sensor in settings["sensors"]:
        codegen_func.sensor_func_h_gen(sensor, template_dir, header_dir)
        codegen_func.sensor_func_cpp_gen(sensor, template_dir, src_dir)
        if sensor["power"] == True:
            codegen_func.sensor_power_h_gen(sensor, template_dir, header_dir)
            codegen_func.sensor_power_cpp_gen(sensor, template_dir, src_dir)

    # Load converter statement
    codegen_func.load_converter_h_gen(template_dir, header_dir)
    codegen_func.load_converter_cpp_gen(template_dir, src_dir)

    # Core statement
    codegen_func.core_h_gen(template_dir, header_dir)
    codegen_func.core_cpp_gen(settings, template_dir, src_dir)
    codegen_func.core_power_h_gen(template_dir, header_dir)
    codegen_func.core_power_cpp_gen(template_dir, src_dir)

    # Main statement
    codegen_func.main_cpp_gen(settings, template_dir, src_dir)

    print("OK!")


# This code will be executed if the script is run directly and not imported as a module
if __name__ == "__main__":
    # Create an argument parser with a description
    parser = argparse.ArgumentParser(description="Generate src code")

    # Add an argument for the input file path
    parser.add_argument(
        "-f",
        "--input-file",
        type=str,
        metavar="PATH",
        help="Path to JSON input simulation settings",
    )

    # Add an argument for the template directory path
    parser.add_argument(
        "-t",
        "--template-dir",
        type=str,
        metavar="PATH",
        default="./templates",
        help="Path to templates directory",
    )

    # Add an argument for the output directory path
    parser.add_argument(
        "-o",
        "--output-dir",
        type=str,
        metavar="PATH",
        default=".",
        help="Path where code will be generated",
    )

    # Parse the command line arguments
    args = parser.parse_args()

    # Convert the input file path to a Path object
    input_file = Path(args.input_file)

    # Convert the template directory path to a Path object
    template_dir = Path(args.template_dir)

    # Convert the output directory path to a Path object
    output_dir = Path(args.output_dir)

    # Call the main function with the parsed arguments
    main(input_file=input_file, template_dir=template_dir, output_dir=output_dir)
