# PLOTTER: A Data Plotting Script

## Overview
This script is designed to read a data file, extract relevant information, and generate a visualization plot of the data. The script is written in Python and utilizes the Pandas library for data manipulation and Matplotlib for plotting.

## Requirements
- Python 3.x
- Pandas
- Matplotlib

## Usage
To run the script, use the following command:

```bash
python plotter.py <data_file>
```

### Arguments
- `<data_file>`: The path to the data file you wish to visualize. The data file should be formatted as a space-separated values file with a timestamp in the first column and at least one additional column of values.

### Example
```bash
python plotter.py sample_data.txt
```

## Data File Format
The data file should be formatted as follows:
- Space-separated values
- First column: Timestamps
- Subsequent columns: Data values

Comments in the file should start with the `%` character. The script expects the first character in the file to be skipped (e.g., a leading comment character).

### Sample Data File
```
% Sample Data
timestamp value1 value2 value3
2021-01-01 10 20 30
2021-01-02 15 25 35
2021-01-03 20 30 40
```

## Features
- Reads a space-separated data file
- Skips the first character of the file (useful for skipping a leading comment character)
- Parses the file and checks for at least two columns
- Plots three specific columns of data against the timestamp (Currently: columns 2 and 3. Could be modified according to necessities)
