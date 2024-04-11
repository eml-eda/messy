# Resolution

A resolution object in MESSY is defined following SystemC timing units, a resolution is comprised of a unit measure (`SC_MS`, `SC_NS`, `SC_PS`) and a multiplier value, below an example that defines 30 seconds:

```JSON
"resolution":{
    "unit":"SC_S",
    "mult":30
}
```