# messy

The availability of simulation platforms is a key ingredient for making RISC-V based hardware solutions pervasive. Simulators represent a cost-effective and scalable solution that enable rapid prototyping without the need for physical hardware. For these reasons, there is a growing availability of programs designed to simulate entire RISC-V chips. Some options include QEMU, Simulink, Renode, and GVSoC. However, these tools present some limitations. Most of them only simulate the computing core with scarce possibilities to perform system-level simulations. Moreover, they simulate only functional features, with poor support for extra-functional properties such as power consumption. This work tries to address these challenges by presenting a flexible simulator.

The simulator combines the SystemC/SistemC-AMS physical and continuous time modelling capabilities with GVSoC, a lightweight and flexible RISC-V functional instruction set simulator. The simulator takes two inputs. The first is a high-level description of the system in JSON which is then internally translated into SystemC/SystemC-AMS components through a template-based code generator written in Python. The second is the C code to be simulated on the RISC-V core through GVSoC.

This is a brief overview of the simulator. For more information, please refer to the thesis available at https://webthesis.biblio.polito.it/28569/

# Documentation
Until the project is not public, you can read the documentation in the following way, supposing you are using Visual Studio Code:

Clone the repository

```bash
git clone https://github.com/eml-eda/messy
```

Change into the directory

```bash
cd messy
```
Open this folder in Visual Studio Code

Create a python virtual environment

```bash
python3 -m venv venv
```

Activate the virtual environment

```bash
source venv/bin/activate
```

Install the dependencies

```bash
pip install -r requirements.txt
```

Serve the documentation

```bash
mkdocs serve
```

Now you see something like this:

```
Serving on http://127.0.0.1:8000/
```

Visual Studio Code should automatically setup port forwarding. So you can simply open a browser and go to [http://127.0.0.1:8000](http://127.0.0.1:8000) to see the documentation.

