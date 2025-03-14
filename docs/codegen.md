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

