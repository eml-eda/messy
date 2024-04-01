# Sensors

Sensors are important components for this simulator. Every sensor can be composed by two separate instances:

- **Functional**: this instance is responsible for define the internal characteristics of the sensor. This instance interfaces with the core through a bus
- **Power**: this instance controls the state of the sensor and exposes its current and voltage

The functional part retrieves details of the request, such as the address, read/write flag, and data from the core. Its logic consists of a structure of if statements where it queries an array of elements (simulated internal registers of the sensor), sets the correct power state, and waits. After this step, the power instance is activated, which updates the power information for a certain amount of time defined by the sensor’s configuration. This is done to simulate the computational time that the sensor takes in real life to analyze the data and update the corresponding register. After that, the power instance sets an IDLE state and gives control back to the functional instance, which can then prepare Data OUT and GO signals to reply to the core.

# Definition

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

Each sensor needs a certain amount of memory to interact with the system, this is simulated and setted thanks to the `register_memory` paramter.

Ideally each sensor could define a set of states and model it in those states, however currently the system supports only the reading state, the writing state and an idle state, which are fundamental.

Each state is defined considering the current(voltage) needed and optionally a timing `delay` information(how much does the sensor stay in state).

Tracing can be set as described in [Tracing](tracing.md).

Finally, the information about the reference voltage can be set through the `vref` parameter.