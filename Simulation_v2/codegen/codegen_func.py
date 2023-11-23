from jinja2 import Template

__all__ = [
    "config_gen",
    "conv_sensor_h_gen",
    "conv_sensor_cpp_gen",
    "main_cpp_gen",
    "sensor_h_gen",
    "sensor_cpp_gen",
    "sensor_func_h",
]


def config_gen(settings, template_dir, output_dir):
    """
    Generate config.h file

    Parameters
    ----------
    settings : dict
        Dictionary containing the simulation settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    nsensor = 0
    for sensor in settings["sensors"]:
        nsensor += 1

    # define generic params
    with open(output_dir / "config.h", "w") as f:
        f.write("//General Parameters\n")
        f.write(f'#define SIM_STEP {settings["sim_step"]}\n')
        f.write(f'#define SIM_LEN {settings["sim_len"]}\n')
        f.write(f'#define SIM_RESOLUTION sc_core::SC_SEC\n')
        f.write(f'#define SOC_INIT {settings["soc_init"]}\n')
        f.write(f'#define VREF_BUS {settings["vref_bus"]}\n')
        f.write(f'#define SELFDISCH_FACTOR {settings["selfdisch_factor"]}\n')
        f.write(f"#define NS {nsensor + 1}\n")
        f.write(f"#define NP {nsensor}\n")
        # Temporary Lines #
        f.write(f"// params for CPU\n")
        f.write(f"#define CPU_I_IDLE 0.002\n")
        f.write(f"#define CPU_T_ON 6\n")
        f.write(f"#define CPU_ON_I_ON 13\n")
        # End temporary Lines #

    # define sensors' params
    with open(template_dir / "h" / "config_sensor.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "config.h", "a") as f:
            f.write(template.render(sensors=settings["sensors"]))


# Main Methods


def main_cpp_gen(settings, template_dir, output_dir):
    """
    Generate main.cpp file

    Parameters
    ----------
    settings : dict
        Dictionary containing the simulation settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "cpp" / "main_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "main.cpp", "w") as f:
            f.write(template.render(settings=settings))


# Core Mehods


def core_h_gen(template_dir, output_dir):
    """
    Generate core.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "core_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "core.h", "w") as f:
            f.write(template.render())


def core_cpp_gen(settings, template_dir, output_dir):
    """
    Generate core.cpp file

    Parameters
    ----------
    settings : dict
        Dictionary containing the simulation settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "core_cpp.txt") as temp:
        template = Template(temp.read())
        tmp_data = dict(settings)
        with open(output_dir / "core.cpp", "w") as f:
            f.write(template.render(data=tmp_data))


def core_power_h_gen(template_dir, output_dir):
    """
    Generate core_power.h file
    
    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "core_power_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "core_power.h", "w") as f:
            f.write(template.render())


def core_power_cpp_gen(template_dir, output_dir):
    """
    Generate core_power.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "core_power_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "core_power.cpp", "w") as f:
            f.write(template.render())


# Functional System Bus Methods


def sys_functional_bus_h_gen(template_dir, output_dir):
    """
    Generate sys_functional_bus.h file
    
    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "sys_functional_bus_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "sys_functional_bus.h", "w") as f:
            f.write(template.render())


def sys_functional_bus_cpp_gen(settings, template_dir, output_dir):
    """
    Generate sys_functional_bus.cpp file

    Parameters
    ----------
    settings : dict
        Dictionary containing the simulation settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "sys_functional_bus_cpp.txt") as temp:
        template = Template(temp.read())
        tmp_data = dict(settings)
        with open(output_dir / "sys_functional_bus.cpp", "w") as f:
            f.write(template.render(data=tmp_data))


# Power System Bus Methods


def sys_power_bus_h_gen(template_dir, output_dir):
    """
    Generate sys_power_bus.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "sys_power_bus_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "sys_power_bus.h", "w") as f:
            f.write(template.render())


def sys_power_bus_cpp_gen(template_dir, output_dir):
    """
    Generate sys_power_bus.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "sys_power_bus_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "sys_power_bus.cpp", "w") as f:
            f.write(template.render())


# Sensor Methods


def sensor_func_h_gen(sensor, template_dir, output_dir):
    """
    Generate sensor.h file

    Parameters
    ----------
    sensor : dict
        Dictionary containing the sensor settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "sensor_func_h.txt") as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        tmp_sensor["name"] = sensor["name"] + "_functional"
        with open(output_dir / f'{tmp_sensor["name"]}.h', "w") as f:
            f.write(template.render(sensor=tmp_sensor))


def sensor_func_cpp_gen(sensor, template_dir, output_dir):
    """
    Generate sensor.cpp file
    
    Parameters
    ----------
    sensor : dict
        Dictionary containing the sensor settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "sensor_functional_cpp.txt") as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        with open(output_dir / f'{tmp_sensor["name"]}_functional.cpp', "w") as f:
            f.write(template.render(sensor=tmp_sensor))


def sensor_power_h_gen(sensor, template_dir, output_dir):
    """
    Generate sensor.h file

    Parameters
    ----------  
    sensor : dict
        Dictionary containing the sensor settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "sensor_power_h.txt") as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        tmp_sensor["name"] = sensor["name"] + "_power"
        with open(output_dir / f'{tmp_sensor["name"]}.h', "w") as f:
            f.write(template.render(sensor=tmp_sensor))


def sensor_power_cpp_gen(sensor, template_dir, output_dir):
    """
    Generate sensor.cpp file

    Parameters
    ----------
    sensor : dict
        Dictionary containing the sensor settings
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "sensor_power_cpp.txt") as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        with open(output_dir / f'{tmp_sensor["name"]}_power.cpp', "w") as f:
            f.write(template.render(sensor=tmp_sensor))


# Load Converter Methods


def load_converter_h_gen(template_dir, output_dir):
    """
    Generate load_converter.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "load_converter_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "load_converter.h", "w") as f:
            f.write(template.render())


def load_converter_cpp_gen(template_dir, output_dir):
    """
    Generate load_converter.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "load_converter_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "load_converter.cpp", "w") as f:
            f.write(template.render())


# Peukert Battery Methods


def battery_peukert_h_gen(template_dir, output_dir):
    """
    Generate battery_peukert.h file
    
    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "battery_peukert_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_peukert.h", "w") as f:
            f.write(template.render())


def battery_peukert_cpp_gen(template_dir, output_dir):
    """
    Generate battery_peukert.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "battery_peukert_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_peukert.cpp", "w") as f:
            f.write(template.render())


# Circuit_Model Battery Methods


def battery_circuit_h_gen(template_dir, output_dir):
    """
    Generate battery_circuit.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "h" / "battery_char_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_char.h", "w") as f:
            f.write(template.render())

    with open(template_dir / "h" / "battery_voc_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_voc.h", "w") as f:
            f.write(template.render())

    with open(template_dir / "h" / "battery_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery.h", "w") as f:
            f.write(template.render())


def battery_circuit_cpp_gen(template_dir, output_dir):
    """
    Generate battery_circuit.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "cpp" / "battery_char_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_char.cpp", "w") as f:
            f.write(template.render())

    with open(template_dir / "cpp" / "battery_voc_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_voc.cpp", "w") as f:
            f.write(template.render())

    with open(template_dir / "cpp" / "battery_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery.cpp", "w") as f:
            f.write(template.render())


# Battery Converter Methods


def battery_converter_h_gen(template_dir, output_dir):
    """
    Generate battery_converter.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """

    with open(template_dir / "h" / "battery_converter_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_converter.h", "w") as f:
            f.write(template.render())

    with open(template_dir / "h" / "config_converter_battery_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "config_converter_battery.h", "w") as f:
            f.write(template.render())


def battery_converter_cpp_gen(template_dir, output_dir):
    """
    Generate battery_converter.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "cpp" / "battery_converter_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "battery_converter.cpp", "w") as f:
            f.write(template.render())


# PV Panel Methods


def pv_panel_h_gen(template_dir, output_dir):
    """
    Generate pv_panel.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "h" / "pv_panel_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "pv_panel.h", "w") as f:
            f.write(template.render())

    with open(template_dir / "h" / "config_pv_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "config_pv.h", "w") as f:
            f.write(template.render())


def pv_panel_cpp_gen(template_dir, output_dir):
    """
    Generate pv_panel.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "cpp" / "pv_panel_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "pv_panel.cpp", "w") as f:
            f.write(template.render())


# PV Panel Converter Methods


def converter_pv_h_gen(template_dir, output_dir):
    """
    Generate converter_pv.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    
    with open(template_dir / "h" / "converter_pv_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "converter_pv.h", "w") as f:
            f.write(template.render())

    with open(template_dir / "h" / "config_converter_pv_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "config_converter_pv.h", "w") as f:
            f.write(template.render())


def converter_pv_cpp_gen(template_dir, output_dir):
    """
    Generate converter_pv.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated
    """
    with open(template_dir / "cpp" / "converter_pv_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "converter_pv.cpp", "w") as f:
            f.write(template.render())


# LUT Methods


def lut_h_gen(template_dir, output_dir):
    """
    Generate lut.h file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated

    """

    with open(template_dir / "h" / "lut_h.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "lut.h", "w") as f:
            f.write(template.render())


def lut_cpp_gen(template_dir, output_dir):
    """
    Generate lut.cpp file

    Parameters
    ----------
    template_dir : pathlib.Path
        Path to templates directory
    output_dir : pathlib.Path
        Path where code will be generated

    """
    
    with open(template_dir / "cpp" / "lut_cpp.txt") as temp:
        template = Template(temp.read())
        with open(output_dir / "lut.cpp", "w") as f:
            f.write(template.render())
