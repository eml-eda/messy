# Getting Started

To simplify the process of getting started, we have provided a docker image that contains all the dependencies needed to run the code. This is the recommended way to get started.

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

To get started, you need to build the docker image. This can be done by running the following command:

```bash
docker build -t sysc-sim:latest .
```

This will build the image and tag it with the name you specify. You can then run the image with the following command:

```bash
docker run -it sysc-sim:latest
```

For addition details on what the Dockerfile does, see [docs/docker.md](docs/docker.md).

