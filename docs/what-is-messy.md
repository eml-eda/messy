# What is MESSY?

MESSY (Multi-layer Extra-functional Simulator for SystemC) is an open-source framework that integrates functional RISC-V simulation with SystemC-AMS to enable detailed analysis of extra-functional properties like power consumption.

## Key Features

- **Integrated Simulation**: Combines functional simulation of RISC-V cores (using GVSoC) with SystemC-AMS for modeling extra-functional aspects.
- **System-Level Power Modeling**: Allows for detailed power modeling of the entire system, including the CPU, sensors, and other components.
- **Modular and Configurable**: Systems are defined using a JSON configuration file, allowing for easy customization and experimentation.
- **Extensible**: Users can add new components and models to the framework.

## How it works

MESSY uses a JSON configuration file to define the system architecture, including the components and their connections. This file is then used to generate the SystemC code that simulates the system. The user can write C/C++ code to be executed by the RISC-V core, which can interact with the simulated hardware components.

The simulation produces detailed output, including power consumption data, which can be used to analyze and optimize the system's energy efficiency.

![alt text](images/messy_full_systemc_double_sensors.png)

## Getting Started

To get started with MESSY, please refer to the [Getting Started](getting-started.md) guide. 

