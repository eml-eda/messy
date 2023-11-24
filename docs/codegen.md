# Codegen

This module will explain the codegen process with its characteristics.

The codegen is a Python script that generates the code for the simulation. It is located in the `Simulation_v2/codegen` folder. The codegen is composed of two files:

- `codegen.py`: this is the main file of the codegen. It contains the call to specific functions that will generate the code of the components of the simulator.
- `codegen_func.py`: this file contains some utility functions, mainly the ones used to generate the code starting from the templates, that are used by the `codegen.py` file.

# Codegen

The `codegen.py` expect a JSON file as input, which contains the configuration for the simulator. 

An example of a JSON file is the following:

```JSON
{
    "sim_step" : 1000,
    "sim_len": 7736400,
    "vref_bus" : 3.3,
    "soc_init" : 1.0,
    "selfdisch_factor" : 0.0,
    "battery" : "circuit",
    "sensors" : [
        {
            "power"  : true,
            "name"   : "air_quality_sensor",
            "voltage": 3.3,
            "reg" : 50,
            "states" : [
                {
                    "name"    : "ON_READ",
                    "current" : "48.2",
                    "time_on" : "30"
                },
                {
                    "name"    : "ON_WRITE",
                    "current" : "49.2",
                    "time_on" : "30"
                },
                {
                    "name"    : "IDLE",
                    "current" : "0.002"
                }
            ]
        }]
}
```

In this case for example, we are describing a single sensor. The meaning of different fields is the following:

- `sim_step`: this is the simulation step
- `sim_len`: this is the simulation length 
- `vref_bus`: this is the voltage of the bus, in Volts.
- `soc_init`: this is the initial state of charge of the battery
- `selfdisch_factor`: this is the self discharge factor of the battery+
- `battery`: this is the type of battery. It can be `circuit` or `peukert`
- `sensors`: this is the list of sensors. Each sensor is described by the following fields:
  - `power`: this is a boolean that indicates if the sensors contains the power instance
  - `name`: this is the name of the sensor
  - `voltage`: this is the voltage of the sensor
  - `reg`: number of sensor’s internal registers
  - `states`: this is the list of states of the sensor. Each state is described by the following fields:
    - `name`: this is the name of the state
    - `current`: this is the current of the state
    - `time_on`: this is the time on of the state 






# Run the codegen

Now we can run the codegen:

```bash
python3 codegen.py -f easy.json
```

If everything went well, you should see the following output:

```bash
OK!
```

It is important to specify the JSON file, since it contains the configuration for the simulator. 

This command will generate the code for the simulation. The code will be generated in the `Simulation_v2/codegen/src` and `Simulation_v2/codegen/inc` folders.

Now, before moving on, we need to move the `src` and `inc` folder into the `Simulation_v2` folder:

```bash
cp -r src ../
cp -r inc ../
```