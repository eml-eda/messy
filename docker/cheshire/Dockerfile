# Start with a base image of ubuntu
FROM ubuntu:22.04

# Update and upgrade the system
RUN apt update && apt upgrade -y

# Install all the needed dependencies
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Rome apt install -y \
    autoconf \
    automake \
    autotools-dev \
    bc \
    bison \
    build-essential \
    clang-format \
    cmake \
    curl \
    device-tree-compiler \
    doxygen \
    flex \
    gawk \
    gdb \
    git \
    gperf \
    graphicsmagick-libmagick-dev-compat \
    graphviz \
    gtkwave \
    libfdt-dev \
    libftdi-dev \
    libftdi1 \
    libgl1-mesa-dev \
    libgmp-dev \
    libjpeg-dev \
    libmpc-dev \
    libmpfr-dev \
    libqt5charts5-dev \
    libqt6serialport6-dev \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsndfile1-dev \
    libtool \
    libusb-1.0-0-dev \
    ninja-build \
    patchutils \
    pkg-config \
    python3-pip \
    qt6-base-dev \
    qtbase5-dev \
    rsync \
    scons \
    sudo \
    texinfo \
    unzip \
    vim \
    wget \
    zlib1g-dev 

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

RUN pip3 install --upgrade pip

RUN pip3 install mako

# Add a user that is the same as the host user
ARG USER_ID
ARG GROUP_ID

# Clone, build and install the riscv-gnu-toolchain for rv64
WORKDIR /

RUN git clone https://github.com/riscv/riscv-gnu-toolchain

WORKDIR /riscv-gnu-toolchain

RUN ./configure --prefix=/opt/riscv

RUN make -j4    

ENV PATH="/opt/riscv/bin:${PATH}"

# Install SystemC 2.3.3 and SystemC AMS 2.3.4
WORKDIR /

RUN git clone https://github.com/eml-eda/systemc-systemc-ams.git

RUN cp systemc-systemc-ams/systemc-2.3.3.tar /systemc.tar 

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
RUN make -j4

# Check SystemC 2.3.3
RUN make check -j4

# Install SystemC 2.3.3
RUN make install -j4

# Change the permissions of the systemc folder
RUN chmod -R 777 /usr/local/systemc

# Change directory to /
WORKDIR /

# Copy the systemc-ams-2.3.4 folder to the container
RUN cp systemc-systemc-ams/systemc-ams-2.3.4.tar /systemc-ams.tar

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
RUN make -j4

# Install SystemC AMS
RUN make install -j4

# Change the permissions of the systemc-ams folder
RUN chmod -R 777 /usr/local/systemc-ams

WORKDIR /

COPY . /messy

WORKDIR /messy

RUN cp /messy/docker/cheshire/Makefile /messy/messy/Makefile

RUN chmod 777 /messy/messy/Makefile

# Create a user with the same user id and group id as the host user
RUN groupadd -g ${GROUP_ID} docker_group && useradd -u ${USER_ID} -g docker_group docker_user

RUN mkdir /home/docker_user
RUN mkdir /home/docker_user/.vscode-server
RUN chmod 777 -R /home/docker_user/.vscode-server

RUN cp /root/.bashrc /home/docker_user/.bashrc

# Switch user
USER docker_user

# Set the default command to launch the desired shell (bash in this case)
CMD ["/bin/bash"]
