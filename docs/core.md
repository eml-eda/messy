The core, alongside the [adapter](adapter.md) is probably the most important component of the whole system. Inside it contains all the necessary functions used to run the simulation. Inside it contains an instance of the `iss_adapter`.

Similarly to most of the other components, the core is composed of two separate instances:

- **Functional**: this instance is responsible for defining the internal characteristics of the core. This instance interfaces with the core through a bus
- **Power**: this instance controls the state of the core and exposes its current and voltage

## Functional Instance

The functional instance is as follows:

![text](images/core_functional.png)

### Signals Summary

A summary of the signals is shown in the table below:

| Signal | Direction | Connected to | Description |
| --- | --- | --- | --- |
| request_go | input | [Functional Bus](functional-bus.md) | |
| request_value | input | [Functional Bus](functional-bus.md) | |
| idx_sensor | input | [Functional Bus](functional-bus.md) | |
| request_address | output | [Functional Bus](functional-bus.md) | |
| request_data | output | [Functional Bus](functional-bus.md) | |
| request_size | output | [Functional Bus](functional-bus.md) | |
| functional_bus_flag | output | [Functional Bus](functional-bus.md) | |
| request_ready | output | [Functional Bus](functional-bus.md) | |
| power_signal | output | Core Power| |

### Input Signals

The input signals are:

- **request_go**: This signal is a boolean. It comes from the [functional bus](functional-bus.md) and is used to signal the core when a request is completed. Initially, request_go is set to 0, and it means that the request is being processed. When the request is completed, the [functional bus](functional-bus.md) sets the request go to 1. The core then wakes up and set the request_ready signal to 0. The [functional bus](functional-bus.md) then frees the sensor and sets the request_go to 0 again.
- **request_value**: This signal is a pointer. It comes from the [sensor](sensors.md), then goes through the [functional bus](functional-bus.md), and finally reaches the core. It contains the pointer to the data written to the sensor memory or read from it. When the request is a write this value is ignored
- **idx_sensor**: This signal is an integer value. It comes from the [functional bus](functional-bus.md) and contains the index of the sensor that is being accessed

### Output Signals

The output signals are:

- **request_address**: This signal is an unsigned integer. It goes to the [functional bus](functional-bus.md) and contains the offset of the sensor memory that is being accessed. For example, if the sensor memory is contains 256 bytes and the request_address is set to 10 it means that we are accessing the 10th byte of the sensor memory. If the request has a size greater than 1, the request address will be the starting address of the memory block of the sensor.
- **request_data**: This signal is a pointer. It goes to the [functional bus](functional-bus.md) and contains the pointer to the data that is being written to the sensor memory. In case of a read request, this value is ignored and not set.
- **request_size**: this signal is an unsigned integer. It goes to the [functional bus](functional-bus.md) and contains the number of bytes that are being read from or written to the sensor memory. For example, if the request_size is set to 4 it means that we are reading or writing 4 elements of the sensor memory (since the sensor memory is byte-addressable).
- **functional_bus_flag**: This signal is a boolean. It goes to the [functional bus](functional-bus.md) and it used to indicate if the request is a read or a write. If the flag is set to true it means that the request is a read, otherwise it is a write.
- **request_ready**: This signal is a boolean. It goes to the [functional bus](functional-bus.md) and it is used to indicate that the request is ready to be processed by the [functional bus](functional-bus.md). In the [functional bus](functional-bus.md), in fact, the main loop is waiting for this signal to be set to true before forwarding the request to the sensor.
- **power_signal**: This signal is a double. It is not used and should be removed

## Power Instance

TODO