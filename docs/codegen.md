# Code Generation

The `messy.py` script uses a JSON configuration file to generate the SystemC/SystemC-AMS models for the simulation. This document explains the structure of the JSON file and the code generation process.

## JSON Configuration File

The JSON file defines the entire system to be simulated. Here is an example of a simple configuration:

```json
{
    "resolution": "SC_MS",
    "tracing": {},
    "bus": {
        "vref": 3.3
    },
    "core": {
        "vref": 1.8,
        "iss": "gvsoc",
        "config": {
            "path": "./gvsoc_config.json",
            "axi_path": "/chip/axi_proxy"
        }
    },
    "peripherals": {
        "sensors": {
            "mic_click": {}
        },
        "harvesters": {
            "battery": {},
            "photovoltaic": {}
        }
    }
}
```

### Top-Level Properties

- `resolution`: The simulation time resolution. See [Resolution](resolution.md) for more details.
- `tracing`: Tracing configuration. See [Tracing](tracing.md) for more details.
- `bus`: The power bus configuration. See [Power Bus](power-bus.md) for more details.
- `core`: The RISC-V core configuration. See [Core](core.md) for more details.
- `peripherals`: The peripherals connected to the core, such as sensors and harvesters.

### Peripherals

The `peripherals` object contains two sub-objects:

- `sensors`: A list of sensors to be included in the simulation. See [Sensors](sensors.md) for more details.
- `harvesters`: A list of harvesters to be included in the simulation. See [Harvesters](harvesters.md) for more details.

## Code Generation Process

The `messy.py` script reads the JSON configuration file and generates the SystemC/SystemC-AMS models based on the templates located in the `messy/codegen/templates` directory. The generated code is placed in the `messy/codegen/src` and `messy/codegen/include` directories.

The generated code is then compiled and linked with the user application to create the final simulation executable.

