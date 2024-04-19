# Start with a base image of ubuntu
FROM ubuntu:22.04

# Update and upgrade the system
RUN apt update && apt upgrade -y

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
    sudo \
    gdb \
    autoconf \
    automake \
    autotools-dev \
    curl \
    libmpc-dev \
    libmpfr-dev \
    libgmp-dev \
    gawk \
    build-essential \
    bison \
    flex \
    texinfo \
    gperf \
    libtool \
    patchutils \
    bc \
    zlib1g-dev \
    build-essential \
    git \
    libftdi-dev \
    libftdi1 \
    doxygen \
    python3-pip \
    libsdl2-dev \
    curl \
    cmake \
    libusb-1.0-0-dev \
    scons \
    gtkwave \
    libsndfile1-dev \
    rsync \
    autoconf \
    automake \
    texinfo \
    libtool \
    pkg-config \
    libsdl2-ttf-dev \
    vim

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 10

# Add a user that is the same as the host user
ARG USER_ID
ARG GROUP_ID

# Change directory to /
WORKDIR /

RUN wget https://github.com/pulp-platform/pulp-riscv-gnu-toolchain/releases/download/v1.0.16/v1.0.16-pulp-riscv-gcc-ubuntu-18.tar.bz2

RUN tar -xf v1.0.16-pulp-riscv-gcc-ubuntu-18.tar.bz2

RUN rm v1.0.16-pulp-riscv-gcc-ubuntu-18.tar.bz2

RUN mv /v1.0.16-pulp-riscv-gcc-ubuntu-18 /pulp_toolchain

WORKDIR /pulp_toolchain

# Make the sourceme.sh file executable
RUN chmod +x sourceme.sh

# Source the sourceme.sh file
RUN /bin/bash -c "source sourceme.sh"

WORKDIR /

RUN git clone --recursive https://github.com/pulp-platform/pulp-sdk.git

WORKDIR /pulp-sdk

RUN git submodule update --remote --recursive

RUN rm -r tools/gvsoc

WORKDIR /pulp-sdk/tools

RUN git clone --recursive https://github.com/eml-eda/gvsoc.git

WORKDIR /pulp-sdk

RUN pip3 install argcomplete pyelftools prettytable six mako rich

ENV PULP_RISCV_GCC_TOOLCHAIN=/pulp_toolchain

RUN /bin/bash -c "source configs/pulp-open.sh"

RUN chmod +x configs/pulp-open.sh

COPY docker/pulp-open/build_open.sh .

RUN /bin/bash -c "source build_open.sh"
# Pass the current directory to the container
WORKDIR /

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

WORKDIR /

COPY . /messy

WORKDIR /messy

RUN cp /messy/docker/pulp-open/Makefile /messy/messy/Makefile

RUN chmod 777 /messy/messy/Makefile

# Create a user with the same user id and group id as the host user
RUN groupadd -g ${GROUP_ID} docker_group && useradd -u ${USER_ID} -g docker_group docker_user

RUN echo "source /pulp-sdk/configs/pulp-open.sh" >> /root/.bashrc

RUN mkdir /home/docker_user
RUN mkdir /home/docker_user/.vscode-server
RUN chmod 777 -R /home/docker_user/.vscode-server

RUN cp /root/.bashrc /home/docker_user/.bashrc

# Switch user
USER docker_user

# Set the default command to launch the desired shell (bash in this case)
CMD ["/bin/bash"]
