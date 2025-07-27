## How to run it

Before running this example, you have launch the docker container. You can find the instructions in the [Docker](../docker.md) page of the documentation.

When you are in the docker container, you have to go to the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open.json -a /messy/examples/read_sensor
```
The parameters are:

1. `-f` is the file name of the chosen system configuration
2. `-a` is the path of the folder of the application

## Explanation
 
This example demonstrates how to read from a sensor's memory using different methods: direct byte access, `memcpy`, and accessing a custom struct. It initializes a memory region and then reads values from it, printing them to the console.

You should see output similar to this:

```
...
Messy uint8 sensor test, there should be 256 iters

[uint8]	0 : 0
[uint8]	1 : 1
...
[uint8]	255 : 255


Messy memcpy sensor test, there should be 2 iters

[memcpy]	0 : 0
[memcpy]	128 : 128


Messy examples with a struct, there should be 128 iters

[struct]	0 : 0
[struct]	1 : 2
[struct]	2 : 4
...
[struct]	127 : 254
```
