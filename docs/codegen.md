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