# Converters

Converters are very important objects in electronics system, they are supported in MESSY currently by a LUT approach, here an example of a converter:

```JSON
"converter" : {
    "input_variable":"voltage",
    "out_dir":false,
    "lut":{
        "input_values":[0.6559,0.7397,0.8363,0.9588,1.0941,1.1907,1.4291,1.7642,1.8544,1.9961,2.4278,2.5954,2.6534,2.7307,2.7758,2.7887,2.9691,3.1688,3.3428,3.5876,3.7874,3.9678,4.1353,4.3673,4.5928,4.7668,4.9729],
        "efficiency_values":[64.92,69.37,74.44,78.79,81.87,83.69,84.77,85.86,86.04,85.95,89.31,90.66,83.14,75.71,70.73,68.46,68.64,68.19,66.83,63.93,61.75,59.31,57.22,54.77,52.42,50.51,47.70]
    }
}
```

The converter is described through the direction, set by `out_dir`, that we want to consider, the `input_variable` that we want to consider, that can be either voltage or current and finally the previously mentioned LUT([LUT](lut.md)), the LUT second values for a converter are the `efficiency_values`, which are the efficiency percentages of a converter.

The modeled converter can be described as a simple Buck DC-DC converter, but we want to support a wider variety of converters. 