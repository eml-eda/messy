# Traces

The system supports several traces, these can be defined in the initial part of the configuration and used by sensors, converters, core and harvesters later on.

The initial configuration can be done as follows:

```JSON
"tracing":{
    "messy_trace":{
        "filename":"messy_trace.log",
        "resolution":{
            "unit":"SC_MS",
            "mult":1
        }
    }
}
```

Here a trace(multiple can be set) of name `messy_trace` is defined, the `filename` parameters define where the trace will be saved and finally the `resolution` describes the logging frequency through a resolution object better described in [Resolution](resolution.md).

To attach a trace to a sensor, harvester or what else during the configuration of that object the user shall set the `tracing` argument and describe for the related trace what to trace, for example below we want to trace the current of a sensor:

```JSON
"tracing":{
    "messy_trace":{
        "mic_click_I":"current"
    }
}
```

Here the tracing is set only for a trace called `messy_trace` and for this trace only one parameter of the sensor is logged, this is the current and is saved as `mic_click_I` on the log.

Currently voltages, current and State of Charge values can be traced quite easily.