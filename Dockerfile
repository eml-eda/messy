# Start with a base image of ubuntu
FROM ubuntu:22.04

# Add a user that is the same as the host user
ARG USER_ID
ARG GROUP_ID

# Update and upgrade the system
RUN apt-get update && apt-get upgrade -y

# Install all the needed dependencies
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Rome apt install -y \
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

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

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
RUN ./configure --prefix=/usr/local/systemc CXXFLAGS="-DSC_CPLUSPLUS=201402L"

# Build SystemC 2.3.3
RUN make

# Check SystemC 2.3.3
RUN make check

# Install SystemC 2.3.3
RUN make install

# Change the permissions of the systemc folder
RUN chmod -R 777 /usr/local/systemc

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
RUN ./configure --with-systemc=/usr/local/systemc --prefix=/usr/local/systemc-ams --disable-systemc_compile_check CXXFLAGS="-DSC_CPLUSPLUS=201402L"

# Build SystemC AMS
RUN make

# Install SystemC AMS
RUN make install

# Change the permissions of the systemc-ams folder
RUN chmod -R 777 /usr/local/systemc-ams

# Change directory to /
WORKDIR /

# Clone the gap_riscv_toolchain_ubuntu repository
RUN git clone https://github.com/GreenWaves-Technologies/gap_riscv_toolchain_ubuntu.git

# Change directory to gap_riscv_toolchain_ubuntu
WORKDIR gap_riscv_toolchain_ubuntu

# Install the gap_riscv_toolchain_ubuntu
RUN ./install.sh /usr/lib/gap_riscv_toolchain

# Change the permissions of the gap_riscv_toolchain_ubuntu folder
RUN chmod -R 777 /usr/lib/gap_riscv_toolchain

# Change directory to /
WORKDIR /

# Copy the gap_sdk_private_correct.zip file to the container
COPY deps/gap_sdk_private_tesi_calice.zip /gap_sdk.zip
# COPY deps/gap_sdk_private-5.14_dev.zip /gap_sdk.zip

# # Extract the gap_sdk.zip file
RUN unzip gap_sdk.zip 

# # Remove the gap_sdk.zip file
RUN rm gap_sdk.zip

# Rename gap_sdk_private_correct to gap_sdk
RUN mv gap_sdk_private_correct gap_sdk 
# RUN mv gap_sdk_private-5.14_dev gap_sdk 

# Change directory to gap_sdk
WORKDIR gap_sdk

# Install the python3 requirements for the gap_sdk
RUN pip3 install -r requirements.txt
RUN pip3 install -r doc/requirements.txt
RUN pip3 install -r tools/nntool/requirements.txt
RUN pip3 install -r utils/gapy_v2/requirements.txt


# Install the requirements for the sysc-sim
RUN pip3 install -r requirements.txt

# Copy the custom sourceme.sh file to the container
COPY custom_sourceme.sh sourceme.sh

# Make the sourceme.sh file executable
RUN chmod +x sourceme.sh

# Source the sourceme.sh file
RUN /bin/bash -c "source sourceme.sh && make all"

# Copy the custom folder containing the custom .c examples files to /gap_sdk/examples/gap9
COPY custom /gap_sdk/examples/gap9

# Change the permissions of the gap_sdk folder
RUN chmod -R 777 /gap_sdk

# Set working directory to /
WORKDIR /home/sysc-sim

# Copy a custom entrypoint script into the container
COPY entrypoint.sh /usr/local/bin/entrypoint.sh

# Set the custom entrypoint script as the entry point for the container
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]

# Give the user the permission to run the custom entrypoint script
RUN chmod +x /usr/local/bin/entrypoint.sh

# Set the default command to launch the desired shell (bash in this case)
CMD ["/bin/bash"]

# Create a user with the same user id and group id as the host user
RUN groupadd -g 1000 docker_group && useradd -u 1000 -g docker_group docker_user