# Docker getting started

## Requirements

To build a Docker image, you need to have Docker installed on your machine. You can find the installation instructions [here](https://docs.docker.com/engine/install/). In addition to that you need to populate the `deps/` directory with the SystemC and SystemC-AMS `.tar` files. You can find them [here](https://www.accellera.org/downloads/standards/systemc) and [here](https://www.coseda-tech.com/systemc-ams-proof-of-concept). The Dockerfile expects the following files:

- `systemc-x.x.x.tar`
- `systemc-ams-x.x.x.tar`

!!! warning 
    The `x.x.x` at the end of the file name of the SystemC and SystemC-AMS files should be replaced by the downloaded version. After that check the `Dockerfile` to make sure to put the correct version in the `COPY` command. You need to change the `x.x.x` in the following line:

    ```Dockerfile
    COPY deps/systemc-2.3.3.tar /systemc.tar
    ...
    COPY deps/systemc-ams-2.3.4.tar /systemc-ams.tar
    ``` 
!!! important
    For compatibility reasons we suggest you to use the following version of SystemC and SystemC-AMS:

    1. SystemC **2.3.3**
    2. SystemC-AMS **2.3.4**


## Overview of the Dockerfile

Each Dockerfile is based on the Ubuntu 22.04 image. It installs the following dependencies:

- SystemC x.x.x ([https://systemc.org/](https://systemc.org/))
- Systemc-AMS x.x.x ([https://www.accellera.org/downloads/standards/systemc](https://www.accellera.org/downloads/standards/systemc))
- Core SDK
- Core RISC-V
- Make
- CMake
- g++
- python3
- python3-pip

## Build the Docker image

The first step is to build a Docker image. For example to build the pulp-open container the following command should be issued:

```bash
docker build . -f docker/pulp-open/Dockerfile -t messy
```

The first time you run this command, it will take some time to build the image. Thanks to docker caching, the next time you run this command, it will be much faster.

## Run the Docker container

Once the image is built, you can run the container with the following command:

```bash
docker run -it --rm -v $(pwd):/messy messy:latest
```

This command will run the container and mount the current directory in the `/messy` directory of the container. This way, you can access the files from the container and the host. Now you should see the shell inside the container.
