# Start with a base image of ubuntu
FROM ubuntu:22.04

# Update and upgrade the system
RUN apt-get update && apt-get upgrade -y

# Install python3 and pip3
RUN apt-get install -y python3 python3-pip

# Install wget and curl
RUN apt-get install -y wget curl

# Install git
RUN apt-get install -y git

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

# Configure SystemC AMS
RUN ./configure --with-systemc=/usr/local/systemc --prefix=/usr/local/systemc-ams --disable-systemc_compile_check

# Build SystemC AMS
RUN make

# Install SystemC AMS
RUN make install

# Change directory to /
WORKDIR /







