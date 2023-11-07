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

For details on what the Dockerfile does and how to build the image, please refer to the [Docker gettin started](docker.md) file.


## Run the codegen

The first step to run this project is to run the codegen. Since they are python script, we first have to create a virtual envinroment. This is done with the command:

```bash
python3 -m venv venv
```

Then we have to activate the virtual envinroment:

```bash
source venv/bin/activate
```

Now we can install the dependencies:

```bash
pip install -r requirements.txt
```

Now we can run the codegen:

```bash
cd Simulation_v2/codegen
python3 codgen.py -f ../parallel.json
```

This command will generate the code for the simulation. The code will be generated in the `Simulation_v2/codegen/src` and `Simulation_v2/codegen/inc` folders.