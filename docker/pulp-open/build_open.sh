#!/bin/bash
pip3 install -r /pulp-sdk/tools/gvsoc/requirements.txt
pip3 install -r /pulp-sdk/tools/gvsoc/gapy/requirements.txt
pip3 install -r /pulp-sdk/tools/gvsoc/core/requirements.txt
cd /pulp_toolchain
source sourceme.sh
cd /pulp-sdk
source configs/pulp-open.sh
make build