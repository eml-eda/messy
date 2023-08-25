from jinja2 import Template

__all__ = [
    'bus_h_gen', 'bus_cpp_gen',
    'config_gen',
    'conv_sensor_h_gen', 'conv_sensor_cpp_gen',
    'main_cpp_gen',
    'makefile_gen',
    'mcu_cpp_gen',
    'rf_cpp_gen',
    'sensor_h_gen', 'sensor_cpp_gen',
    'sensor_func_h'
]


def bus_h_gen(settings, template_dir, output_dir):
    with open(template_dir / 'h' / 'bus_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'bus.h', 'w') as f:
            active_sensors = settings['sensors']
            sensor_names = []
            for sensor in active_sensors:
                sensor_names.append(sensor['name'])
            f.write(template.render(active_sensors=sensor_names))


def bus_cpp_gen(settings, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'bus_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'bus.cpp', 'w') as f:
            active_sensors = settings['sensors']
            sensor_names = []
            for sensor in active_sensors:
                sensor_names.append(sensor['name'])
            f.write(template.render(active_sensors=sensor_names))


def config_gen(settings, template_dir, output_dir):
    # define generic params
    with open(output_dir / 'config.h', 'w') as f:
        f.write('// generic simulation settings\n')
        f.write(f'#define SIM_STEP {settings["sim_step"]}\n')
        f.write(f'#define SIM_LEN {settings["sim_len"]}\n')
        f.write(f'#define PERIOD {settings["period"]}\n')
        f.write(f'#define VREF_BUS {settings["vref_bus"]}\n')
        f.write(f'#define SOC_INIT {settings["soc_init"]}\n')
        f.write(f'#define SELFDISCH_FACTOR {settings["selfdisch_factor"]}\n')

    # define sensors' params
    active_sensors = settings['sensors']
    with open(template_dir / 'h' / 'config_sensor.txt') as temp:
        template = Template(temp.read())
        max_sensing_time = 0    # used to know when the MCU can start
        for sensor in active_sensors:
            sensor_name = sensor['name']
            current_on = sensor['current_on']
            current_idle = sensor['current_idle']
            activation_time = sensor['activation_time']
            time_on = sensor['time_on']
            with open(output_dir / 'config.h', 'a') as f:
                f.write(
                    template.render(
                        sensor_name=sensor_name,
                        current_on=current_on,
                        current_idle=current_idle,
                        activation_time=activation_time,
                        time_on=time_on))
            sensing_time = int(activation_time) + int(time_on)
            if sensing_time > max_sensing_time:
                max_sensing_time = sensing_time

    # define MCU's params
    with open(template_dir / 'h' / 'config_MCU.txt') as temp:
        template = Template(temp.read())
        mcu = settings['mcu']
        mcu_i_idle = mcu['current_idle']
        states = mcu['states']
        mcu_act_time = max_sensing_time + 1  # mcu activated when sensing ends
        act_times = {}
        for state in states:
            val = int(mcu_act_time) + int(state['time_on']) + 1
            act_times[state['name']] = val
        with open(output_dir / 'config.h', 'a') as f:
            f.write(
                template.render(
                    mcu_i_idle=mcu_i_idle,
                    mcu_act_time=mcu_act_time,
                    states=states,
                    act_times=act_times))

    # define params for RF
    with open(template_dir / 'h' / 'config_RF.txt') as temp:
        template = Template(temp.read())
        rf = settings['rf']
        states = rf['states']
        rf_i_idle = rf['current_idle']
        with open(output_dir / 'config.h', 'a') as f:
            f.write(
                template.render(
                    rf_i_idle=rf_i_idle,
                    states=states))

def main_cpp_gen(settings, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'main_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'main.cpp', 'w') as f:
            active_sensors = settings['sensors']
            sensor_names = []
            for sensor in active_sensors:
                sensor_names.append(sensor['name'])
            f.write(template.render(active_sensors=sensor_names))


def makefile_gen(settings, template_dir, output_dir):
    with open(template_dir / 'Makefile' / 'Makefile.txt') as temp:
        template = Template(temp.read())
        with open(output_dir, 'w') as f:
            active_sensors = settings['sensors']
            sensor_names = []
            sensor_id = 0
            for sensor in active_sensors:
                sensor_id += 1
                sensor_names.append(sensor_id)
            f.write(template.render(active_sensors=sensor_names))


def mcu_cpp_gen(settings, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'mcu_cpp.txt') as temp:
        template = Template(temp.read())
        states = settings['mcu']['states']
        with open(output_dir / 'mcu.cpp', 'w') as f:
            f.write(template.render(states=states))


def rf_cpp_gen(settings, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'rf_cpp.txt') as temp:
        template = Template(temp.read())
        states = settings['rf']['states']
        with open(output_dir / 'rf.cpp', 'w') as f:
            f.write(template.render(states=states))

# Functional System Bus Methods

def sys_functional_bus_h_gen(template_dir, output_dir):
    with open(template_dir / 'h' / 'sys_functional_bus_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'sys_functional_bus.h', 'w') as f:
            f.write(template.render())

# Power System Bus Methods

def sys_power_bus_h_gen(template_dir, output_dir):
    with open(template_dir / 'h' / 'sys_power_bus_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'sys_power_bus.h', 'w') as f:
            f.write(template.render())

#Sensor Methods

def sensor_func_h_gen(sensor, template_dir, output_dir):
    with open(template_dir / 'h' / 'sensor_func_h.txt') as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        tmp_sensor['name'] = sensor["name"]+"_functional"
        with open(output_dir / f'{tmp_sensor["name"]}.h', 'w') as f:
            f.write(template.render(sensor=tmp_sensor))

def sensor_func_cpp_gen(sensor, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'sensor_functional_cpp.txt') as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        with open(output_dir / f'{tmp_sensor["name"]}_functional.cpp', 'w') as f:
            f.write(template.render(sensor=tmp_sensor))

def sensor_power_h_gen(sensor, template_dir, output_dir):
    with open(template_dir / 'h' / 'sensor_power_h.txt') as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        tmp_sensor['name'] = sensor["name"]+"_power"
        with open(output_dir / f'{tmp_sensor["name"]}.h', 'w') as f:
            f.write(template.render(sensor=tmp_sensor))

def sensor_power_cpp_gen(sensor, template_dir, output_dir):
    with open(template_dir / 'cpp' / 'sensor_power_cpp.txt') as temp:
        template = Template(temp.read())
        tmp_sensor = dict(sensor)
        with open(output_dir / f'{tmp_sensor["name"]}_power.cpp', 'w') as f:
            f.write(template.render(sensor=tmp_sensor))

#Load Converter Methods

def load_converter_h_gen(template_dir, output_dir):
    with open(template_dir / 'h' / 'load_converter_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'load_converter.h', 'w') as f:
            f.write(template.render())

def load_converter_cpp_gen(template_dir, output_dir):
    with open(template_dir / 'cpp' / 'load_converter_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'load_converter.cpp', 'w') as f:
            f.write(template.render())

#Peukert Battery Methods

def battery_peukert_h_gen(template_dir, output_dir):
    with open(template_dir / 'h' / 'battery_peukert_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_peukert.h', 'w') as f:
            f.write(template.render())

def battery_peukert_cpp_gen(template_dir, output_dir):
    with open(template_dir / 'cpp' / 'battery_peukert_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_peukert.cpp', 'w') as f:
            f.write(template.render())

#Circuit_Model Battery Methods

def battery_circuit_h_gen(template_dir, output_dir):
    with open(template_dir / 'h' / 'battery_char_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_char.h', 'w') as f:
            f.write(template.render())

    with open(template_dir / 'h' / 'battery_voc_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_voc.h', 'w') as f:
            f.write(template.render())

    with open(template_dir / 'h' / 'battery_h.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery.h', 'w') as f:
            f.write(template.render())

def battery_circuit_cpp_gen(template_dir, output_dir):
    with open(template_dir / 'cpp' / 'battery_char_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_char.cpp', 'w') as f:
            f.write(template.render())

    with open(template_dir / 'cpp' / 'battery_voc_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery_voc.cpp', 'w') as f:
            f.write(template.render())

    with open(template_dir / 'cpp' / 'battery_cpp.txt') as temp:
        template = Template(temp.read())
        with open(output_dir / 'battery.cpp', 'w') as f:
            f.write(template.render())