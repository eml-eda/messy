# Getting Started

This document will guide you through the steps to get started with the project.

## Clone the repository

First, clone the repository to your local machine:

```bash
git clone https://github.com/eml-eda/messy
cd messy
```

## Dependencies

To build the project, you need to have Docker installed on your machine. You can find the installation instructions [here](https://docs.docker.com/engine/install/).

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

## Run some examples

There are a few examples in `examples` folder. Try one of them:

- [Hello World](examples/hello-world.md)
- [Periodic Sensor](examples/periodic-sensor.md)