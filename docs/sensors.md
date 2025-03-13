# Sensors

Sensors are important components for this simulator. Every sensor can be composed by two separate instances:

- **Functional**: this instance is responsible for define the internal characteristics of the sensor. This instance interfaces with the core through a bus
- **Power**: this instance controls the state of the sensor and exposes its current and voltage

## Functional Instance
The functional instance is as follows: 

![text](images/sensor_functional.png)

### Signals Summary
A summary of the signals is shown in the table below:

| Signal | Direction | Connected to | Description |
| --- | --- | --- | --- |
| enable | input |  | Enable the sensor |
| address | input | Functional bus | Address of the register to read or write |
| data_in | input | Functional bus | Pointer to the input data |
| req_size | input | Functional bus | Size of the data to read or write |
| flag_wr | input | Functional bus | Flag to define if the sensor is going to read or write |
| ready | input | Functional bus | Flag to define if the sensor is ready to read or write |
| data_out | output | Functional bus | Pointer to the output data |
| go | output | Functional bus | Signal that the sensor has completed the operation |
| power_signal | output | Power bus | Power state of the sensor |

### Input Signals
The input signals are:

- **enable**: this signal is used to enable the sensors. If the sensor is enable it will execute read or write operations and it will set the power state accordingly. Otherwise, the sensor is in a state that consumes 0 power.

    ``` c++
    if (enable.read() == true) {
        // read or write operations
    } else {
        power_signal.write(0); // (1)!
    }
    ```
    
    1. If the sensor is disabled, it will consume 0 power.

- **address**: This signal is used to select the register of the sensor to read or write. For example, this is the code when reading:

    ``` c++
    unsigned int add = address.read(); // (1)!

    data_out.write(register_memory + add); // (2)!
    ```
    
    1. Read the address from the bus
    2. Write to the output bus the pointer of the memory of the sensor at address `add`.

- **data_in**: This signal represents the **pointer** to the input data. This is done in order to allow the possibility of passing multiple data to the sensor
- **req_size**: This signal is used to define the size of the data that the sensor is going to read or write.
- **flag_wr**: This signal is used to define if the sensor is going to read or write. If the signal is true, the sensor is going to write, otherwise it is going to read.
- **ready**: This signal is used to define if the sensor is ready to read or write. 

### Output Signals
The output signals are:

- **data_out**: This signal represents the **pointer** to the output data. This signal goes to the functional bus
- **go**: This signal is used to signal that the sensor has completed the operation. It is set to `true` when the sensor has finished, to `false` otherwise. This signal goes to the functional bus
- **power_signal**: This signal represents the power state of the sensor. It is connected to the power instance of the sensor

## Power Instance

The functional instance is a SystemC-AMS Module and is structured as follows: 

![text](images/sensor_power.png)

### Signals Summary
A summary of the signals is shown in the table below:

| Signal | Direction | Connected to | Description |
| --- | --- | --- | --- |
| func_signal | input | Sensor Functional | Power state of the sensor |
| voltage_state | output | Power bus & Load Converter | Voltage of the sensor |
| current_state | output | Power bus | Current of the sensor |

### Input Signals

The input signals are:

- **func_signal**: This signal is used to define the power state of the sensor. It is usually an integer, representing a value from an enum. This signal comes from the functional instance of the sensor.

### Output Signals
The output signals are:

- **voltage_state**: This signal is used to define the voltage of the sensor. This signal is connected to the power bus and the load converter
- **current_state**: This signal is used to define the current of the sensor. This signal is connected to the power bus
## Definition

The sensors are defined together with the rest of the system in a JSON configuration file, below for example there is defined a microphone sensor called `mic_click`:

```JSON
"mic_click": {
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
```

Each sensor needs a certain amount of memory to interact with the system and this is simulated and setted thanks to the `register_memory` parameter. 


Ideally each sensor could define a set of states and model it in those states, however currently the system supports only the reading state, the writing state and an idle state. 

Each state is defined considering the `current`, in mA, needed and optionally a timing `delay` in ms (how much time does the sensor stay in that state).

Tracing can be set as described in [Tracing](tracing.md).

Finally, the information about the reference voltage can be set through the `vref` parameter.
