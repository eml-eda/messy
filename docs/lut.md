# Lookup Tables (LUTs)

Lookup Tables (LUTs) are used in MESSY to model the behavior of various components, such as converters and sensors. They provide a flexible way to define non-linear relationships between two variables.

## JSON Configuration

A LUT is defined as a JSON object with two arrays: `input_values` and a second array that depends on the component being modeled.

```json
"lut": {
    "input_values": [0.6559, ...],
    "efficiency_values": [64.92, ...]
}
```

- **`input_values`**: An array of input values for the component.
- **`efficiency_values` / `voltage_values` / `current_values`**: An array of corresponding output values. The name of this array depends on the component being modeled.

## How it works

The LUT is implemented as a C++ class that performs linear interpolation to find the output value for a given input value. This allows for a more accurate representation of the component's behavior than a simple linear function.
