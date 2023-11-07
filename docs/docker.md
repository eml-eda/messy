# Docker getting started

## Requirements

To build the Docker image, you need to have Docker installed on your machine. You can find the installation instructions [here](https://docs.docker.com/engine/install/). In addition to that you need to populate the `deps/` directory with the SystemC and SystemC-AMS `.tar` files. You can find them [here](https://www.accellera.org/downloads/standards/systemc) and . The Dockerfile expects the following files:

- `systemc-x.x.x.tar`
- `systemc-ams-x.x.x.tar`
- `gap_sdk_private_correct.zip`

!!! warning 
    The `x.x.x` at the end of the file name of the SystemC and SystemC-AMS files should be replaced by the downloaded version. After that check the `Dockerfile` to make sure to put the correct version in the `COPY` command. You need to change the `x.x.x` in the following line:

    ```Dockerfile
    COPY deps/systemc-2.3.3.tar /systemc.tar
    ...
    COPY deps/systemc-ams-2.3.4.tar /systemc-ams.tar
    ```

## Overview of the Dockerfile

The Dockerfile is based on the Ubuntu 20.04 image. It installs the following dependencies:

- SystemC x.x.x ([https://systemc.org/](https://systemc.org/))
- Systemc-AMS x.x.x ([https://www.accellera.org/downloads/standards/systemc](https://www.accellera.org/downloads/standards/systemc))
- GAP SDK (Custom version based on [https://github.com/GreenWaves-Technologies/gap_sdk](https://github.com/GreenWaves-Technologies/gap_sdk))
- GAP RISC-V Toolchain ([https://github.com/GreenWaves-Technologies/gap_gnu_toolchain](https://github.com/GreenWaves-Technologies/gap_gnu_toolchain))
- Make
- CMake
- g++
- python3
- python3-pip

## Build the Docker image

The first step is to build the Docker image. To do so, run the following command:

```bash
docker build -t sysc-sim:latest .
```

The first time you run this command, it will take some time to build the image. Thanks to docker caching, the next time you run this command, it will be much faster.

## Run the Docker container

Once the image is built, you can run the container with the following command:

```bash
docker run -it --rm -v $(pwd):/home/sysc-sim sysc-sim:latest
```

This command will run the container and mount the current directory in the `/home/sysc-sim` directory of the container. This way, you can access the files from the container and the host.
