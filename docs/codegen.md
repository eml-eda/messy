# Codegen

This module will explain the codegen process with its characteristics.

The codegen is a Python script that generates the code for the simulation. It is located in the `messy/codegen` folder. The codegen is composed of two files:

- `codegen.py`: this is the main file of the codegen. It contains the call to specific functions that will generate the code of the components of the simulator.

# Codegen explanation

The `codegen.py` expect a JSON file as input, which contains the configuration for the simulator. 

An example of a JSON file is the following:

```JSON
{
    "resolution":"SC_MS",
    "tracing":{
        ...
    },
    "bus":{
        "vref":3.3
    },
    "core":{
        "vref":1.8,
        "iss":"gvsoc",
        "config":{
            "path":"./gvsoc_config.json",
            "axi_path":"/chip/axi_proxy"
        },
        ...
    },
    "peripherals":{
        "sensors" : {
            "mic_click": {
                ...
            }
        },
        "harvesters":{
            "battery" : {
                ...
            },
            "photovoltaic" : {
                ...
            }
        }
    }
}
```

In this case for example, we are describing a single sensor. The meaning of different fields is the following:

- `resolution`: this is the simulation resolution using `sc_unit` values. For more details refer to [Resolution](resolution.md)
- `tracing`: tracing is better described in [Tracing](tracing.md) 
- `bus`: the bus object contains all the relative informations about it, which currently resides only in its reference voltage in Volts.

- `core`: definition of the simulated core (system), to define one we can use the following parameters
    - `vref`: reference voltage in Volts
    - `config`: configuration of the ISS, which considers path of files and address of the AXI currently 

- `converters`: Messy also supports converters. Read more at [Converters](converters.md)

- `peripherals`: set of sensors (better described [Sensors](sensors.md)) and harvesters (better described in [Harvesters](harvesters.md))

## Further Details

If we expand the configuration of a sensor, we can see something like this:

```json
    "peripherals":{
        "sensors" : {
            "mic_click": {
                "power": true,
                "tracing":{
                    "messy_trace":{
                        "mic_click_I":"current"
                    }
                },
                "vref": 3.3,
                "register_memory" : 256,
                "states":{
                    "read" : {
                        "current" : "0.12",
                        "delay" : "30"
                    },
                    "write": {
                        "current" : "0.16",
                        "delay" : "30"
                    },
                    "idle":{
                        "current" : "0.002"
                    }
                }
            }
        },
```

## Memory

As we can see, there is a field called `register_memory`. This takes into account the memory space of the sensor (it can be seen as a MMIO). This is managed by the following lines of code in the `codegen.py`:

```python
for idx,(sensor_name,sensor) in enumerate(settings["peripherals"]["sensors"].items()):
    sensor["base"] = baseaddress
    sensor["ID"] = idx
    baseaddress += 1 + sensor["register_memory"]
```

This loops over all the sensors in the JSON configuration file and assign the `baseaddress` to that sensor. For this reason, if we want one sensor before the other in the memory, is importan to define them correctly. The `baseaddress` is not to be confused with the `AXI_BASE`. In fact, GVSoC has some memory space that is fixed and cannot be touched, otherwise it causes segmantation faults. After that memory space, there is available memory space where the user can write whatever is needed.

```c
...
#define AXI_BASE 0x20000000
...
int main(void)
{
    ...

    int* mic_click_sensor = (volatile int *)AXI_BASE+0x0;
    ...
}
```

As we can see in the code example (taken from the `read_sensor.c`), to define the address of the `mic_click_sensor` we just need to add `0x0` to the `AXI_BASE_ADDRESS`. This is because the used configuration, which is the `pulp_open.json`, only contains one sensor. 

Supposing we have two sensors with a `memory_map` of 256 and we want to access the memory of the second one, we should add to `AXI_BASE` the offset (which is 256) with the respect to the starting address. 

| Name | Base Address | Memory Register Size | Offset w.r.t AXI_BASE |
| ----| ----| ----|----|
| AXI_BASE| 0x20000000 |-| 0|
| sensor1| 0x20000000|256| 0|
| sensor2 |0x20000256|256| 256|

## States

Another important detail is the presence of different states. At the moment each sensor only supports 3 states:

1. Read: the sensor is reading from the memory
2. Write: the sensor is writing to the memory
3. Idle: the sensor is idle

Inside this states we can define the current drawn to execute that operation and the delay. The delay of the cycles follows the same resolution of the global resolution of the system, which is defined at the beginning of the JSON file.

```JSON
{
    "resolution":"SC_MS",
    ...
}
```

!!! warning
    Right now, messy only supports global configuration for the resolution of the delay of the sensors.

These states, at the moment, are managed in the following way:

- Off
    - Idle
        - Read
            - Idle
        - Write
            - Idle

The state machine can be represented as follows:

``` mermaid
stateDiagram
  state Off
  state Write
  state Read
  state Idle

  [*] --> Off
  Off --> Read
  Off --> Write
  Read --> Idle
  Write --> Idle
  Idle --> Read
  Idle --> Write
```

Future works, will go in the direction of supporting the `Off` state after a certain amount of idle time.