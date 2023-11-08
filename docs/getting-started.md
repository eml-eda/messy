# Getting Started

This document will guide you through the steps to get started with the project.

## Clone the repository

First, clone the repository to your local machine:

```bash
git clone https://github.com/eml-eda/sysc-sim
```

Then change into the directory:

```bash
cd sysc-sim
```

## Build the docker image

For details on what the Dockerfile does and how to build the image, please refer to the [Docker getting started](docker.md) file.


## Run the codegen

When you have completed the previous steps, you should be inside the container. If not, please refer to the [Run the Docker container](docker.md#run-the-docker-container) section of the Docker getting started file.

The shell inside the container should be in the `/home/sysc-sim` directory. From there, you can change into the `Simulation_v2/codegen` directory:

```bash
cd Simulation_v2/codegen
```

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
mv src ../
mv inc ../
```

## Build the GAP SDK

Now we need to build the GAP SDK. To do so, we need to change into the `gap_sdk` directory:

```bash
cd /gap_sdk
```

Then we need to run the command:

```bash
source sourceme.sh
```

and select 1 as the option.

Now we are ready to run the `make` command:

```bash
make all
```

This command will take some time to complete. Once it is done, we can move on to the next step.

## Compiling the GAP application

Now we need to compile the GAP application. To do so, we need to change into the `examples/gap9` directory:

```bash
cd /gap_sdk/examples/gap9
```

Here you will find some folders containing the examples. For this tutorial, we will use the `helloworld` example, that is located in the `basic/helloworld` folder. First, we need to change into that directory:

```bash
cd basic/helloworld
```

Now we can run the `make` command:

```bash
make all run platform=gvsoc
```

This command will take some time to complete. Once it is done, we should see a new folder called `BUILD` in the `helloworld` folder. Move inside the `BUILD` and all its nested folders. You will end up in a folder where will find some files. The one we are interested in are:

- `gvsoc_config.json`:
- `chip.soc.mram.bin`:
- `efuse_preload.data`: 

We need to copy these files in the `Simulation_v2/bin` folder:

```bash
cp gvsoc_config.json /home/sysc-sim/Simulation_v2/bin
cp chip.soc.mram.bin /home/sysc-sim/Simulation_v2/bin
cp efuse_preload.data /home/sysc-sim/Simulation_v2/bin
```

## Build the simulation

Now we can build the simulation. To do so, we need to change into the `Simulation_v2` directory:

```bash
cd /home/sysc-sim/Simulation_v2
```

Then we can run the `make` command:

```bash
make
```

This command will take some time to complete. Once it is done, it creates a `run.x` file in the `Simulation_v2/bin` folder. This is the executable file of the simulation.

## Run the simulation

Now we can run the simulation. To do so, we need to change into the `Simulation_v2/bin` directory:

```bash
cd /home/sysc-sim/Simulation_v2/bin
```

First we have to make the `run.x` file executable:

```bash
chmod +x run.x
```

Then we can run the `run.x` file:

```bash
./run.x
```


