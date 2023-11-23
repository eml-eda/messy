# Codegen

This module will explain the codegen process with its characteristics.

The codegen is a Python script that generates the code for the simulation. It is located in the `Simulation_v2/codegen` folder. The codegen is composed of two files:

- `codegen.py`: this is the main file of the codegen. It contains the call to specific functions that will generate the code of the components of the simulator.
- `codegen_func.py`: this file contains some utility functions, mainly the ones used to generate the code starting from the templates, that are used by the `codegen.py` file.

# Codegen

The `codegen.py` expect a JSON file as input, which contains the configuration for the simulator. 

# Run the codegen

Now we can run the codegen:

```bash
python3 codegen.py -f easy.json
```

If everything went well, you should see the following output:

```bash
OK!
```

It is important to specify the JSON file, since it contains the configuration for the simulator. 

This command will generate the code for the simulation. The code will be generated in the `Simulation_v2/codegen/src` and `Simulation_v2/codegen/inc` folders.

Now, before moving on, we need to move the `src` and `inc` folder into the `Simulation_v2` folder:

```bash
cp -r src ../
cp -r inc ../
```