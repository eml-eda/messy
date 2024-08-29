import argparse
from pathlib import Path

import json
import xml.etree.ElementTree as ET
import os
from ipxact import *

from typing import List

from mako.template import Template
from mako import exceptions

class Template_generator:
    def __init__(self,template_file,out_file,data_to_template) -> None:
        self.template_file = str(template_file)
        self.out_file = str(out_file)
        self.data_to_template = data_to_template

def generate_at_from_with(template_generators: List[Template_generator]):
    for template_gen in template_generators:
        print(f"Generating {template_gen.out_file}")
        template_file = Template(filename=template_gen.template_file)
        code=""
        error_codegen=False
        try:
            code=template_file.render(**template_gen.data_to_template)
        except:
            code=exceptions.html_error_template().render()
            error_codegen=True
        with open(template_gen.out_file+(".html" if error_codegen else ""),"wb" if error_codegen else "w") as f:
            f.write(code)
        if error_codegen:
            print("ERROR WHILE GENERATING CODE!")
            break

def get_signals(settings):
    signals=dict()
    if settings["type"]=="core":
        for param_name,param_type in settings["params"].items():
            signals[param_name]=param_type+"_core"
    if settings["type"]=="converter_core":
        for params_name,param_type in settings["params"].items():
            signals[param_name]="core_conv_"+param_type
    if settings["type"]=="sensor":
        for param_name,param_type in settings["params"].items():
            signals[param_name]=settings["name"]+"_I_S_to_C"
    if settings["type"]=="harvester":
        for param_name,param_type in settings["params"].items():
            signals[param_name]=param_type+"_"+settings["name"]
    if settings["type"]=="converter_harvester":
        for param_name,param_type in settings["params"].items():
            signals[param_name]=param_type+"_"+settings["name"]
    return signals


def main(input_file, input_folder, template_dir, output_dir):
    """
    Main function for code generation

    Parameters
    ----------
    input_file : Path
        Path to the main ipxact designs
    input_folder : Path
        Path to the folder with the ipxact library
    template_dir : Path
        Path to templates directory
    output_dir : Path
        Path where code will be generated
    """

    lib = read_ipxact_design(input_file)
    design = dict()

    design["peripherals"] = {}
    design["peripherals"]["harvesters"] = {}
    design["peripherals"]["sensors"] = {}

    for component in lib["components"]:
        path_to_component = os.path.join(input_folder, (component["instanceName"] + "_component.xml"))
        if (component["componentRef"]["name"] == "bus"):
            design = process_bus(path_to_component, component["componentRef"], design)
        elif (component["componentRef"]["name"] == "core"):
            design = process_core(path_to_component, component["componentRef"], design)
        elif (component["componentRef"]["name"] == "core_converter"):
            design["core"] = process_converter(path_to_component, component["componentRef"], design["core"])
        elif (component["componentRef"]["name"] == "battery"):
            design["peripherals"]["harvesters"] = process_battery(path_to_component, component["componentRef"], design["peripherals"]["harvesters"])
        elif (component["componentRef"]["name"] == "battery_converter"):
            design["peripherals"]["harvesters"]["battery"] = process_converter(path_to_component, component["componentRef"], design["peripherals"]["harvesters"]["battery"])
        elif (component["componentRef"]["name"] == "photovoltaic"):
            design["peripherals"]["harvesters"] = process_photovoltaic(path_to_component, component["componentRef"], design["peripherals"]["harvesters"])
        elif (component["componentRef"]["name"] == "photovoltaic_converter"):
            design["peripherals"]["harvesters"]["photovoltaic"] = process_converter(path_to_component, component["componentRef"], design["peripherals"]["harvesters"]["photovoltaic"])
        elif ("_sensor" in component["componentRef"]["name"]):
            design["peripherals"]["sensors"] = process_sensor(path_to_component, component["componentRef"], design["peripherals"]["sensors"])

    settings = design
    
    settings["utils"]={
        "get_signals":get_signals,
    }

    base_header_dir = Path(os.path.join(output_dir, "include"))
    base_header_dir.mkdir(parents=True, exist_ok=True)
    base_src_dir = Path(os.path.join(output_dir, "src"))
    base_src_dir.mkdir(parents=True, exist_ok=True)

    converter_header_dir = Path(os.path.join(output_dir, "include", "converter"))
    converter_header_dir.mkdir(parents=True, exist_ok=True)
    converter_src_dir = Path(os.path.join(output_dir, "src", "converter"))
    converter_src_dir.mkdir(parents=True, exist_ok=True)

    adapters_header_dir = Path(os.path.join(output_dir, "include", "adapters"))
    adapters_header_dir.mkdir(parents=True, exist_ok=True)
    adapters_src_dir = Path(os.path.join(output_dir, "src", "adapters"))
    adapters_src_dir.mkdir(parents=True, exist_ok=True)

    template_generators = []
    # Adding internal variable
    baseaddress = 0
    if "tracing" not in settings:
        settings["tracing"]={}
    for trace_val in settings["tracing"].values():
        trace_val["traces"]=[]
    if "tracing" in settings["core"]:
        for t_name,t_params in settings["core"]["tracing"].items():
            settings["tracing"][t_name]["traces"].append({
                "type":"core",
                "name":"core",
                "params":t_params
            })
    if "peripherals" not in settings:
        settings["peripherals"]={"sensors":{},"harvesters":{}}
    if "sensors" not in settings["peripherals"]:
        settings["peripherals"]["sensors"]={}
    if "harvesters" not in settings["peripherals"]:
        settings["peripherals"]["harvesters"]={}
    for idx,(sensor_name,sensor) in enumerate(settings["peripherals"]["sensors"].items()):
        sensor["base"] = baseaddress
        sensor["ID"] = idx
        baseaddress += sensor["register_memory"]
        if "tracing" in sensor:
            for t_name,t_params in sensor["tracing"].items():
                if t_name in settings["tracing"]:
                    settings["tracing"][t_name]["traces"].append({
                        "type":"sensor",
                        "name":sensor_name,
                        "params":t_params
                    })
        template_generators.append(Template_generator(os.path.join(template_dir, "include", "sensor_functional.hpp"),
                                   os.path.join(base_header_dir, f"sensor_{sensor_name}_functional.hpp"),
                                   {"sensor_name":sensor_name,**sensor}))
        template_generators.append(Template_generator(os.path.join(template_dir,"src","sensor_functional.cpp"),
                                   os.path.join(base_src_dir,f"sensor_{sensor_name}_functional.cpp"),
                                   {"sensor_name":sensor_name,**sensor}))
        
        template_generators.append(Template_generator(os.path.join(template_dir,"include","sensor_power.hpp"),
                                   os.path.join(base_header_dir,f"sensor_{sensor_name}_power.hpp"),
                                   {"sensor_name":sensor_name,**sensor}))
        template_generators.append(Template_generator(os.path.join(template_dir, "src", "sensor_power.cpp"),
                                   os.path.join(base_src_dir, f"sensor_{sensor_name}_power.cpp"),
                                   {"sensor_name":sensor_name,**sensor}))
        

    for idx,(harvester_name,harvester) in enumerate(settings["peripherals"]["harvesters"].items()):
        if "tracing" in harvester:
            for t_name,t_params in harvester["tracing"].items():
                if t_name in settings["tracing"]:
                    settings["tracing"][t_name]["traces"].append({
                        "type":"harvester",
                        "name":harvester_name,
                        "params":t_params
                    })
        template_generators.append(Template_generator(os.path.join(template_dir, "src", "harvester.cpp"),
                                   os.path.join(base_src_dir, f"harvester_{harvester_name}.cpp"),
                                   {"harvester_name":harvester_name,**harvester}))
        template_generators.append(Template_generator(os.path.join(template_dir, "include", "harvester.hpp"),
                                   os.path.join(base_header_dir, f"harvester_{harvester_name}.hpp"),
                                   {"harvester_name":harvester_name,**harvester}))
        if "converter" in harvester and "lut" in harvester["converter"]:
            if "tracing" in harvester["converter"]:
                for t_name,t_params in harvester["converter"]["tracing"].items():
                    if t_name in settings["tracing"]:
                        settings["tracing"][t_name]["traces"].append({
                            "type":"harvester_converter",
                            "name":harvester_name,
                            "params":t_params
                        })
            template_generators.append(Template_generator(os.path.join(template_dir, "src", "converter", "lut_converter.cpp"),
                                   os.path.join(base_src_dir, "converter", f"{harvester_name}_converter.cpp"),
                                   {"name":harvester_name,"unit":"harvester",**harvester,**harvester["converter"]}))
            template_generators.append(Template_generator(os.path.join(template_dir, "include", "converter", "lut_converter.hpp"),
                                   os.path.join(base_header_dir, "converter", f"{harvester_name}_converter.hpp"),
                                   {"name":harvester_name,"unit":"harvester",**harvester,**harvester["converter"]}))
    
    # Config.h statement
    template_generators.append(Template_generator(os.path.join(template_dir, "include", "config.hpp"),
                               os.path.join(base_header_dir, "config.hpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "include", "messy_request.hpp"),
                               os.path.join(base_header_dir, "messy_request.hpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "src", "messy_request.cpp"),
                               os.path.join(base_src_dir, "messy_request.cpp"),
                               {**settings}))

    template_generators.append(Template_generator(os.path.join(template_dir, "include", "lut.hpp"),
                               os.path.join(base_header_dir, "lut.hpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "src", "lut.cpp"),
                               os.path.join(base_src_dir, "lut.cpp"),
                               {**settings}))

    template_generators.append(Template_generator(os.path.join(template_dir, "include", "functional_bus.hpp"),
                               os.path.join(base_header_dir, "functional_bus.hpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "src", "functional_bus.cpp"),
                               os.path.join(base_src_dir, "functional_bus.cpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "include", "power_bus.hpp"),
                               os.path.join(base_header_dir, "power_bus.hpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "src", "power_bus.cpp"),
                               os.path.join(base_src_dir, "power_bus.cpp"),
                               {**settings}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "include", "converter", "load_converter.hpp"),
                               os.path.join(base_header_dir, "converter", "load_converter.hpp"),
                               {**settings}))

    template_generators.append(Template_generator(os.path.join(template_dir, "src", "converter", "load_converter.cpp"),
                               os.path.join(base_src_dir, "converter", "load_converter.cpp"),
                               {**settings}))
    
    if "converter" in settings["core"] and "lut" in settings["core"]["converter"]:
        if "tracing" in settings["core"]["converter"]:
            for t_name,t_params in settings["core"]["converter"]["tracing"].items():
                settings["tracing"][t_name]["traces"].append({
                    "type":"converter_core",
                    "name":"core",
                    "params":t_params
                })
        template_generators.append(Template_generator(os.path.join(template_dir, "src", "converter", "lut_converter.cpp"),
                                os.path.join(base_src_dir, "converter", "core_converter.cpp"),
                                {"name":"core","unit":"core",**settings["core"],**settings["core"]["converter"]}))
        template_generators.append(Template_generator(os.path.join(template_dir, "include", "converter", "lut_converter.hpp"),
                                os.path.join(base_header_dir, "converter", "core_converter.hpp"),
                                {"name":"core","unit":"core",**settings["core"],**settings["core"]["converter"]}))
    
    #template_generators.append(Template_generator(template_dir+"include"+"adapters"+"iss_adapter.hpp",
    #                           adapters_header_dir+"iss_adapter.hpp",
    #                           {**settings}))
    
    adapter_used={
        "adapter_class":"ISS_Adapter",
        "adapter_filenames":"iss_adapter"
    }

    if "iss" in settings["core"] and settings["core"]["iss"]=="gvsoc":
        adapter_used={
            "adapter_class":"AdapterGvsoc",
            "adapter_filenames":"adapter_gvsoc"
        }
    if "iss" in settings["core"] and settings["core"]["iss"]=="gvsoc_gap9":
        adapter_used={
            "adapter_class":"AdapterGvsocGap9",
            "adapter_filenames":"adapter_gvsoc_gap9"
        }
    if adapter_used["adapter_filenames"]!="iss_adapter":
        template_generators.append(Template_generator(os.path.join(template_dir, "include", "adapters", f"{adapter_used['adapter_filenames']}.hpp"),
                                os.path.join(adapters_header_dir, f"{adapter_used['adapter_filenames']}.hpp"),
                                {**settings,**settings["core"]}))

        template_generators.append(Template_generator(os.path.join(template_dir, "src", "adapters", f"{adapter_used['adapter_filenames']}.cpp"),
                                os.path.join(adapters_src_dir, f"{adapter_used['adapter_filenames']}.cpp"),
                                {**settings,**settings["core"]}))

    template_generators.append(Template_generator(os.path.join(template_dir, "include", "core.hpp"),
                               os.path.join(base_header_dir, "core.hpp"),
                               {**settings,**settings["core"],**adapter_used}))

    template_generators.append(Template_generator(os.path.join(template_dir, "src", "core.cpp"),
                               os.path.join(base_src_dir, "core.cpp"),
                               {**settings,**settings["core"]}))
    
    template_generators.append(Template_generator(os.path.join(template_dir, "include", "core_power.hpp"),
                               os.path.join(base_header_dir, "core_power.hpp"),
                               {**settings,**settings["core"]}))

    template_generators.append(Template_generator(os.path.join(template_dir, "src", "core_power.cpp"),
                               os.path.join(base_src_dir, "core_power.cpp"),
                               {**settings,**settings["core"]}))

    template_generators.append(Template_generator(os.path.join(template_dir, "src", "main.cpp"),
                               os.path.join(base_src_dir, "main.cpp"),
                               {**settings}))

    generate_at_from_with(template_generators=template_generators)


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
        help="Path to the top-level IPXACT design",
    )

    # Add an argument for the template directory path
    parser.add_argument(
        "-t",
        "--template-dir",
        type=str,
        metavar="PATH",
        default="./codegen/templates",
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
    input_file = os.path.join(os.getcwd(), Path(args.input_file))

    # Convert the template directory path to a Path object
    input_folder = Path(args.input_file).parent

    # Convert the template directory path to a Path object
    template_dir = os.path.join(os.getcwd(),Path(args.template_dir))

    # Convert the output directory path to a Path object
    output_dir = os.path.join(os.getcwd(),Path(args.output_dir))

    # Call the main function with the parsed arguments
    main(input_file=input_file, input_folder=input_folder, template_dir=template_dir, output_dir=output_dir)
