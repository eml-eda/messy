#!/bin/bash

## This script runs the Messy code generation and execution for the Cheshire project. ##

APP="/tests/fake_sensor_multiple_reads.spm.fpga.elf"

cd messy 

# Build codegen
make codegen file=./codegen/cheshire.json -j8
if [ $? -ne 0 ]; then
    echo "Code generation failed"
    exit 1
fi

# Copy application binaries
make application-cheshire app=$APP
if [ $? -ne 0 ]; then
    echo "Copying application binaries failed"
    exit 1
fi

# Build and run Messy
make run -j8
if [ $? -ne 0 ]; then
    echo "Running Messy failed"
    exit 1
fi
