# Adding Custom Code to the GAP SDK

The GAP SDK contains some examples. You can see them at the following link: [https://github.com/GreenWaves-Technologies/gap_sdk/tree/master/examples/gap8](https://github.com/GreenWaves-Technologies/gap_sdk/tree/master/examples/gap8). 

For our application, we need to add custom program. For this reason, to simplify the process, there is a folder called `custom` that contains all the custom code that we need to add to the GAP SDK.

This folder is copied, during the build of the Docker container, to the GAP SDK, at `/gap_sdk/examples/gapx/` (where x is the version of GAP), and if the user wants to run them, the process is exactly the same as the one already described in [Compiling the GAP application](getting-started.md#compiling-the-gap-application).

An improvement would be to map these folder as a volume, so that the user can modify the code without the need to rebuild the Docker container. This is left as future work.