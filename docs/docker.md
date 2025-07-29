!!! note
    This is a more detailed explanation of the Dockerfile. For a quick start, please refer to the [Getting Started](getting-started.md) guide.

## Requirements

To build a Docker image, you need to have Docker installed on your machine. You can find the installation instructions [here](https://docs.docker.com/engine/install/).

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

The first step is to build a Docker image. For example to build the `pulp-open` container the following command should be issued:

```bash
docker build . -f docker/pulp-open/Dockerfile -t messy --build-arg USER_ID=$(id -u ${USER}) --build-arg GROUP_ID=$(id -g ${USER})
```

The first time you run this command, it will take some time to build the image. Thanks to docker caching, the next time you run this command, it will be much faster.

## Run the Docker container

Once the image is built, you can run the container with the following command:

```bash
docker run -it --rm -v $(pwd):/messy messy:latest
```

This command will run the container and mount the current directory in the `/messy` directory of the container. This way, you can access the files from the container and the host. Now you should see the shell inside the container.
