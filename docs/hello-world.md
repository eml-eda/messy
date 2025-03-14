Before running this example, you have launch the docker container. You can find the instructions in the [Docker](docker.md) page of the documentation.

When you are in the docker container, you have to go to the `messy` directory:

```bash
cd /messy/messy/
```

Then we can run python:

```bash
python3 messy.py -f codegen/pulp_open.json -a /messy/examples/helloworld
```
The parameters are:

1. `-f` is the file name of the chosen system configuration
2. `-a` is the path of the folder of the application

This code will build and execute the `helloworld` example. You should see the following output:

```
Entering main controller
[31 0] Hello World!
Cluster master core entry
[0 7] Hello World!
[0 6] Hello World!
[0 5] Hello World!
[0 4] Hello World!
[0 3] Hello World!
[0 2] Hello World!
[0 0] Hello World!
[0 1] Hello World!
Cluster master core exit
Bye !
```

<!-- ## Messy.py Explanation

The `messy.py` python scripts expects 2 arguments:

1. The first one is the file name of the chosen configuration. In the example, the basic pulp-open configuration is used.
2. The second one if the path of the folder of the application.

MESSY, during the build process, also the C++ SystemC-AMS sources of the used configuration (sensors, batteries etc.). Afterward it builds everything. The last step is to gather the configuration of the Instruction Set Simulator (ISS) and runs the simulation.

This python script, will then launch as a subprocesses some `Makefile` targets. Many RISC-V Instruction Set Simulators require to have a configuration file (i.e. memory and peripheral structure). These subprocesses will generate all of them. At the end of the build process, you should have 3 files inside the folder under `/messy/messy/gvsoc_deps`:

1. `gvsoc_config.json`
2. `hyperflash.bin`
3. `spiflash.bin` -->


