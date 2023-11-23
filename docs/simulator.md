## Build the simulation

Now we can build the simulation. To do so, we need to change into the `Simulation_v2` directory:

```bash
cd /home/sysc-sim/Simulation_v2
```

Then we can run the `make` command:

```bash
make
```

This command will take some time to complete. Once it is done, it creates a `run.x` file in the `Simulation_v2/bin` folder. This is the executable file of the simulation.

## Run the simulation

Now we can run the simulation. To do so, we need to change into the `Simulation_v2/bin` directory:

```bash
cd /home/sysc-sim/Simulation_v2/bin
```

First we have to make the `run.x` file executable:

```bash
chmod +x run.x
```
Then we have to up one level:

```bash
cd ..
```

Then we can run the `run.x` file:

```bash
./bin/run.x
```