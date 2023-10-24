# Details about the Dockerfile

The Dockerfile is based on the Ubuntu 20.04 image. It installs the following dependencies:

- SystemC 2.3.3
- Make
- g++
- python3
- python3-pip

The first step is to install all the needed dependencies and updating the system. Then the Dockerfile expects the user to have the SystemC and SystemC-AMS `.tar` files in the `deps/` directory. Thanks to this the Dockerfile is able to copy them in the container and install them.  