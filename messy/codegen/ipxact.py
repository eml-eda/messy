import xml.etree.ElementTree as ET
import json

def parse_element(element):
    parsed_data = {}
    # If the element has text, add it to the dictionary
    if element.text and element.text.strip():
        return element.text.strip()

    # If the element has attributes, add them to the dictionary
    if element.attrib:
        for key in element.attrib:
            parsed_data[key.replace("{http://www.w3.org/2001/XMLSchema-instance}", "")] = element.attrib[key]
    
    # If the element has subelements, recursively parse them
    for child in element:
        child_name = child.tag.replace("{http://www.accellera.org/XMLSchema/IPXACT/1685-2022}", "")
        child_data = parse_element(child)
        if child_name not in parsed_data:
            parsed_data[child_name] = child_data
        else:
            # If the tag name already exists, convert it to a list
            if not isinstance(parsed_data[child_name], list):
                parsed_data[child_name] = [parsed_data[child_name]]
            parsed_data[child_name].append(child_data)
    
    return parsed_data

def xml_to_json(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()
    parsed_data = parse_element(root)
    return {root.tag.replace("{http://www.accellera.org/XMLSchema/IPXACT/1685-2022}", ""): parsed_data}

def read_ipxact_design(file_path):
    json_data = xml_to_json(file_path)
    json_data = json_data["design"]
    
    # Iterate over the child elements and print their details
    lib = {}
    lib["vendor"] = json_data["vendor"]
    lib["version"] = json_data["version"]
    lib["library"] = json_data["library"]
    lib["components"] = json_data["componentInstances"]["componentInstance"]

    return lib

def read_params(params, design):
    if type(params) == dict:
        params = [params]

    for param in params:
        if param["type"] == "float":
            design[param["name"]] = float(param["value"])
        elif param["type"] == "string":
            if (param["value"] == "true"):
                design[param["name"]] = True
            elif (param["value"] == "false"):
                design[param["name"]] = False
            else:
                design[param["name"]] = param["value"]
        elif param["type"] == "int":
            design[param["name"]] = int(param["value"])
    
    return design

def sanity_check(json_data, requirements):
    #Some sanity check to make sure the component is from the same vendor/library and of the same version
    if (json_data["vendor"] != requirements["vendor"]):
        raise Exception(f"Bus vendor of the component {json_data["vendor"]} is not the same as mentioned in the design {requirements["vendor"]}")
    if (json_data["library"] != requirements["library"]):
        raise Exception(f"Bus library of the component {json_data["library"]} is not the same as mentioned in the design {requirements["library"]}")
    if (json_data["version"] != requirements["version"]):
        raise Exception(f"Bus version of the component {json_data["version"]} is not the same as mentioned in the design {requirements["version"]}")

def process_bus(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    sanity_check(json_data, requirements)
    
    design["resolution"] = json_data["vendorExtensions"]["resolution"][0]
    design["tracing"] = {"messy_trace":{"filename":json_data["vendorExtensions"]["filename"], "resolution":json_data["vendorExtensions"]["resolution"][1]}}
    design["bus"] = read_params(json_data["model"]["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], {})

    return design

def process_core(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    design["core"] = {}

    sanity_check(json_data, requirements)
    
    design["core"] = read_params(json_data["model"]["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], design["core"])
    
    design["core"]["config"] = json_data["vendorExtensions"]["config"]
    design["core"]["tracing"] = {"messy_trace" : json_data["vendorExtensions"]["messy_trace"]}

    return design

def process_battery(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    design[requirements["name"]] = {}

    sanity_check(json_data, requirements)

    design[requirements["name"]] = read_params(json_data["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], design[requirements["name"]])
    
    for key in json_data["vendorExtensions"]:
        if key != "class":
            design[requirements["name"]][key] = json_data["vendorExtensions"][key]

    design[requirements["name"]]["self_discharge"]["value"] = float(design[requirements["name"]]["self_discharge"]["value"])
    design[requirements["name"]]["self_discharge"]["resolution"]["mult"] = int(design[requirements["name"]]["self_discharge"]["resolution"]["mult"])

    return design

def process_photovoltaic(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    design["photovoltaic"] = {}

    sanity_check(json_data, requirements)
    
    design["photovoltaic"] = read_params(json_data["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], design["photovoltaic"])
    
    design["photovoltaic"]["iref"] = json_data["vendorExtensions"]["iref"]
    design["photovoltaic"]["vref"] = json_data["vendorExtensions"]["vref"]

    design["photovoltaic"]["iref"]["lut"]["input_values"] = design["photovoltaic"]["iref"]["lut"]["input_values"].strip("[]").split(",")
    design["photovoltaic"]["iref"]["lut"]["input_values"] = [int(i) for i in design["photovoltaic"]["iref"]["lut"]["input_values"]]
    design["photovoltaic"]["iref"]["lut"]["current_values"] = design["photovoltaic"]["iref"]["lut"]["current_values"].strip("[]").split(",")
    design["photovoltaic"]["iref"]["lut"]["current_values"] = [float(i) for i in design["photovoltaic"]["iref"]["lut"]["current_values"]]

    design["photovoltaic"]["vref"]["lut"]["input_values"] = design["photovoltaic"]["vref"]["lut"]["input_values"].strip("[]").split(",")
    design["photovoltaic"]["vref"]["lut"]["input_values"] = [int(i) for i in design["photovoltaic"]["vref"]["lut"]["input_values"]]
    design["photovoltaic"]["vref"]["lut"]["voltage_values"] = design["photovoltaic"]["vref"]["lut"]["voltage_values"].strip("[]").split(",")
    design["photovoltaic"]["vref"]["lut"]["voltage_values"] = [float(i) for i in design["photovoltaic"]["vref"]["lut"]["voltage_values"]]
    
    return design

def process_converter(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    design["converter"] = {}

    sanity_check(json_data, requirements)
    
    design["converter"] = read_params(json_data["model"]["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], design["converter"])
    
    design["converter"]["lut"] = {}
    design["converter"]["lut"]["input_values"] = json_data["vendorExtensions"]["lut"]["input_values"].strip("[]").split(",")
    design["converter"]["lut"]["efficiency_values"] = json_data["vendorExtensions"]["lut"]["efficiency_values"].strip("[]").split(",")
    
    design["converter"]["lut"]["input_values"] = [float(i) for i in design["converter"]["lut"]["input_values"]]
    design["converter"]["lut"]["efficiency_values"] = [float(i) for i in design["converter"]["lut"]["efficiency_values"]]
    
    return design


def process_sensor(file_path, requirements, design):
    json_data = xml_to_json(file_path)
    json_data = json_data["component"]

    design[requirements["name"]] = {}

    sanity_check(json_data, requirements)

    design[requirements["name"]] = read_params(json_data["model"]["instantiations"]["componentInstantiation"]["moduleParameters"]["moduleParameter"], design[requirements["name"]])

    if type(json_data["vendorExtensions"]["states"]["state"]) is dict:
        json_data["vendorExtensions"]["states"]["state"] = [json_data["vendorExtensions"]["states"]["state"]]
    
    design[requirements["name"]]["states"] = {}
    
    for state in json_data["vendorExtensions"]["states"]["state"]:
        for key in state:
            design[requirements["name"]]["states"][key] = state[key]
            if "current" in state[key]:
                design[requirements["name"]]["states"][key]["current"] = float(design[requirements["name"]]["states"][key]["current"])
            if "delay" in state[key]:
                design[requirements["name"]]["states"][key]["delay"] = float(design[requirements["name"]]["states"][key]["delay"])

    return design
