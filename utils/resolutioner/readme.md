# PV Panel Trace File Reading Frequency Workaround

## Overview

The current photovoltaic (PV) panel simulator reads one line from the PV panel trace file based on the system resolution set in the configuration file. If the resolution is set to seconds, each value in the trace file must represent samplings taken at one-second intervals. To match a different resolution, a workaround involves repeating each value in the trace file to achieve the desired resolution.

## Script: resolutioner.py

### Description

`resolutioner.py` is a simple Python script designed to adjust the sampling resolution of a PV panel trace file by repeating each line a specified number of times. The argument `<num_repeats>` should be calculated as:

```
num_repeats = old_resolution / new_resolution
```

### Usage

1. Ensure you have Python installed on your system.
2. Open your terminal or command prompt.
3. Run the script with the following command:

```bash
python resolutioner.py <input_file> <output_file> <num_repeats>
```

- <input_file>: The path to the PV panel trace file.
- <output_file>: The path to the output file where the adjusted resolution trace will be saved.
- <num_repeats>: The number of times each line should be repeated to match the desired resolution.

#### Example
If the original resolution is 1 second and you want to change it to 0.1 seconds, you would set num_repeats to 10:

```bash
python resolutioner.py trace_file.txt adjusted_trace_file.txt 10
```
