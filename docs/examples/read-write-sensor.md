## How to run it

Before running this example, you have launch the docker container. You can find the instructions in the [Docker](../docker.md) page of the documentation.

When you are in the docker container, you have to go to the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open.json -a /messy/examples/read_write_sensor
```
The parameters are:

1. `-f` is the file name of the chosen system configuration
2. `-a` is the path of the folder of the application

## Explanation
 
This example demonstrates a simple read-write operation to a single sensor's memory. It writes a sequence of values to the sensor and then reads them back, printing the results to the console.

You should see output similar to this:

```
...
Microphone sensor 10
Microphone sensor 11
```
