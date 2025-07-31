#!/bin/bash
# This script runs the Messy code generation and execution for the Cheshire project.

clear
cd messy

# Generate standard Messy components from codegen .json file
make codegen file=./codegen/cheshire.json -j8
if [ $? -ne 0 ]; then
    echo "Error during codegen"
    exit 1
fi

make run -j8
if [ $? -ne 0 ]; then
    echo "Error during run"
    exit 1
fi
