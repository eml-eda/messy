## How to run it

Before running this example, you have launch the docker container. You can find the instructions in the [Docker](../docker.md) page of the documentation.

When you are in the docker container, you have to go to the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open.json -a /messy/examples/periodic_sensors
```
The parameters are:

1. `-f` is the file name of the chosen system configuration
2. `-a` is the path of the folder of the application

## Explanation

This example, as the name suggest, is a simple example that periodically writes a value to the sensor memory. This example is useful to understand how to interact with the sensor memory and how the flow of a write operation works. For more information about the write request, please refer to the [Communication Example](../communication-example.md) page.

By adding some print statements inside the functional model of the sensor, we can see how the memory of the sensor is updated, each time a write occurs.

```
[ITER 0/2880] 0.00 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 2
[ITER 1/2880] 0.01 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 4
[ITER 2/2880] 0.02 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 6
[ITER 3/2880] 0.02 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 8
[ITER 4/2880] 0.03 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 10
[ITER 5/2880] 0.04 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 12
[ITER 6/2880] 0.05 Hours Elapsed
[SENSOR] Writing data to register memory at address 0
[SENSOR] Data written: 14
....
```

If we let this example run for enough time and inspect the `messy_trace.log` we can see the battery level decreasing over time. This is because the sensor is periodically writing to the memory, which in turn is consuming energy from the battery. This is a simple example to show how the sensor memory can be accessed and how the sensor can be used to interact with the system.