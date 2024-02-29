# Getting Started

This document will guide you through the steps to get started with the project.

## Clone the repository

First, clone the repository to your local machine:

```bash
git clone https://github.com/eml-eda/messy
```

Then change into the directory:

```bash
cd messy
```

## Build the docker image

For details on what the Dockerfile does and how to build the image, please refer to the [Docker getting started](docker.md) file.


## Run the codegen

When you have completed the previous steps, you should be inside the container. If not, please refer to the [Run the Docker container](docker.md#run-the-docker-container) section of the Docker getting started file.

The shell inside the container should be in the `/home/messy` directory. From there, you can change into the `Simulation_v2/codegen` directory:

```bash
cd Simulation_v2/codegen
```

The codegen has been simplified thanks to the use of Makefiles. To run the codegen, you just need to run the `make` command:

```bash
make codegen file=filename.json
```

For example, if you want to run the codegen on the `easy.json` file, you need to run the following command:

```bash
make codegen file=easy.json
```

## Build the GAP SDK

The building of the GAP_SDK has been directly embedded into the docker image. So you don't need to build it. However, if for some reason you want to build it from scratch, please refer to the [Build the GAP SDK](build-gap-sdk.md) file.

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
cp gvsoc_config.json /home/messy/Simulation_v2/
cp chip.soc.mram.bin /home/messy/Simulation_v2/
cp efuse_preload.data /home/messy/Simulation_v2/
```

## Simulator

Also for this step, everything has been embedded into the Makefile. So we just need to run the `make run` command:

```bash
make run
```

This command will clean, compile and run the simulation. Once it is done, you should see the simulation running.
