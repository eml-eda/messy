# Converters

Converters are essential components in electronic systems, and MESSY provides support for them through a lookup table (LUT) based approach. This allows for the modeling of various types of converters, such as Buck DC-DC converters.

## JSON Configuration

A converter is defined within the JSON configuration file. Here is an example of a converter definition:

```json
"converter": {
    "input_variable": "voltage",
    "out_dir": false,
    "lut": {
        "input_values": [0.6559, 0.7397, 0.8363, 0.9588, 1.0941, 1.1907, 1.4291, 1.7642, 1.8544, 1.9961, 2.4278, 2.5954, 2.6534, 2.7307, 2.7758, 2.7887, 2.9691, 3.1688, 3.3428, 3.5876, 3.7874, 3.9678, 4.1353, 4.3673, 4.5928, 4.7668, 4.9729],
        "efficiency_values": [64.92, 69.37, 74.44, 78.79, 81.87, 83.69, 84.77, 85.86, 86.04, 85.95, 89.31, 90.66, 83.14, 75.71, 70.73, 68.46, 68.64, 68.19, 66.83, 63.93, 61.75, 59.31, 57.22, 54.77, 52.42, 50.51, 47.70]
    }
}
```

- **`input_variable`**: This parameter specifies whether the converter's efficiency is dependent on the input `"voltage"` or `"current"`.
- **`out_dir`**: This boolean parameter determines the direction of the conversion. If `false`, the efficiency is calculated as `Pin / Pout`. If `true`, the efficiency is calculated as `Pout / Pin`.
- **`lut`**: This object defines the lookup table for the converter's efficiency. It contains two arrays:
    - **`input_values`**: The input voltage or current values.
    - **`efficiency_values`**: The corresponding efficiency values in percentage.

## How it works

The converter model uses the specified `input_variable` to look up the efficiency in the `lut`. The efficiency is then used to calculate the output power based on the `out_dir` parameter. This allows for the accurate modeling of power conversion losses in the system. 