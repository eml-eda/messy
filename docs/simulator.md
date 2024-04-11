## Usage

To run a simulation you must go into the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open.json -a /application 
```

The python scripts expects 2 arguments, where the first one is the file name of the configuration chosen, as below, where the basic pulp-open configuration is used.

The second one instead regards the application to simulate folder.

Many RISC-V Instruction Set Simulators require to have a configuration file, and others that may refer to memory flashing etc., for example GvSoC requires a configuration file called gvsoc_config.json and other dependencies.

Therefore, to build a complete workflow MESSY builds these dependencies and completes an end-to-end simulation process.

To recap, MESSY generates the C++ SystemC-AMS sources of our configuration(sensors, batteries etc.) and build, finally it brings the configurations of the ISS and runs the simulation.