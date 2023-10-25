# Start with a base image of ubuntu
FROM ubuntu:22.04

# Update and upgrade the system
RUN apt-get update && apt-get upgrade -y

# Install all the needed dependencies
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Rome apt update && \
    DEBIAN_FRONTEND=noninteractive TZ=Europe/Rome apt install -y \
    autoconf \
    automake \
    bison \
    build-essential \
    cmake \
    curl \
    device-tree-compiler \
    doxygen \
    flex \
    git \
    graphicsmagick-libmagick-dev-compat \
    graphviz \
    gtkwave \
    libfdt-dev \
    libftdi-dev \
    libftdi1 \
    libjpeg-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsndfile1-dev \
    libtool \
    libusb-1.0-0-dev \
    ninja-build \
    pkg-config \
    python3-pip \
    qtbase5-dev \
    libqt5charts5-dev \
    rsync \
    texinfo \
    wget \
    qt6-base-dev \
    libqt6serialport6-dev \
    libgl1-mesa-dev \
    unzip \
    sudo

# Install python3 and pip3
RUN apt-get install -y python3

# Install C++ compiler
RUN apt-get install -y g++

# Install make
RUN apt-get install -y make

# Pass the current directory to the container
COPY deps/systemc-2.3.3.tar /systemc.tar 

# Create a directory for systemc
RUN mkdir /systemc

# Extract the systemc.tar file avoiding the parent directory
RUN tar -xvf systemc.tar -C /systemc --strip-components=1

# Remove the systemc.tar file
RUN rm systemc.tar

# Change directory to systemc
WORKDIR systemc

# Configure SystemC
RUN ./configure --prefix=/usr/local/systemc

# Build SystemC 2.3.3
RUN make

# Check SystemC 2.3.3
RUN make check

# Install SystemC 2.3.3
RUN make install

# Change directory to /
WORKDIR /

# Copy the systemc-ams-2.3.4 folder to the container
COPY deps/systemc-ams-2.3.4.tar /systemc-ams.tar

# Create a directory for systemc-ams
RUN mkdir /systemc-ams

# Extract the systemc-ams-2.3.4.tar file avoiding the parent directory
RUN tar -xvf systemc-ams.tar -C /systemc-ams --strip-components=1

# Remove the systemc-ams-2.3.4.tar file
RUN rm systemc-ams.tar

# Change directory to systemc-ams-2.3.4
WORKDIR systemc-ams

# # Configure SystemC AMS
# RUN ./configure --with-systemc=/usr/local/systemc --prefix=/usr/local/systemc-ams --disable-systemc_compile_check

# # Build SystemC AMS
# RUN make

# # Install SystemC AMS
# RUN make install

# Change directory to /
WORKDIR /

# Clone the gap_riscv_toolchain_ubuntu repository
RUN git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu.git

# Change directory to gap_riscv_toolchain_ubuntu
WORKDIR gap_riscv_toolchain_ubuntu

# Install the gap_riscv_toolchain_ubuntu
RUN ./install.sh

# Change directory to /
WORKDIR /

# Copy the gap_sdk_private_correct.zip file to the container
COPY deps/gap_sdk_private_correct.zip /gap_sdk.zip

# Extract the gap_sdk.zip file
RUN unzip gap_sdk.zip

# Remove the gap_sdk.zip file
RUN rm gap_sdk.zip

# Clone the gap_sdk repository
RUN git clone https://github.com/GreenWaves-Technologies/gap_sdk.git

# Change directory to gap_sdk
WORKDIR gap_sdk

# Make the sourceme.sh file executable
RUN chmod +x sourceme.sh

# Set the default shell to bash
SHELL ["/bin/bash", "-c"] 

# Install the gap_sdk
RUN ./sourceme.sh 1

# Install the python3 requirements for the gap_sdk
RUN pip3 install -r requirements.txt
RUN pip3 install -r doc/requirements.txt
# RUN pip3 install -r tools/nntool/requirements.txt


