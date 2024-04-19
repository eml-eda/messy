# LUT

LookUp Tables are used in a wide variety of applications, in MESSY they are used to help describe converters and input values files.

A LUT is defined through a JSON object containing as following:

```JSON
"lut":{
    "input_values":[0.6559,...],
    "second_variable":[64.92,...]
}
```

In this object the user shall set in `input_values` an array of values representing the input variable(tension, current, nits, torque etc.) and finally should call the second field (`efficiency_values`, `voltage_values` and `current_values`) with an array of the other values.