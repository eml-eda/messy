## How to run it

Before running this example, you have launch the docker container. You can find the instructions in the [Docker](../docker.md) page of the documentation.

When you are in the docker container, you have to go to the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open_double_sensor.json -a /messy/examples/read_write_multiple_sensors
```
The parameters are:

1. `-f` is the file name of the chosen system configuration (note: this example uses `pulp_open_double_sensor.json` as it involves multiple sensors)
2. `-a` is the path of the folder of the application

## Explanation
 
This example demonstrates reading from one sensor and writing to another, and then reading back from the second sensor. It highlights how to interact with multiple sensor instances within the simulation environment.

You should see output similar to this:

```
...
Entering main controller
[0 0] Hello World!
Iter 0
Microphone sensor 0
Iter 1
Microphone sensor 0
...
Iter 9
Microphone sensor 0
Iter_puppo 0
Microphone sensor 2 0
Iter_puppo 1
Microphone sensor 2 1
...
Iter_puppo 9
Microphone sensor 2 9
```
