# IPXACT

This module will explain how the IPXACT design is constructed from a .json design.

More about IPXACT can be read [here](https://www.accellera.org/images/downloads/standards/ip-xact/IP-XACT_User_Guide_2018-02-16.pdf)

This example is made for the following design - `messy/messy/codegen/gap9_pv_panel.json`:

## Introduction

The IPXACT format assumes the presence of multiple files to describe one design. 

The main file is a design file `messy/codegen/ipxact/gap9_pv_panel/gap9_design.xml`, it describes all the components present and their interconnections.

An example of a component instance:

```XML
<ipxact:componentInstance>
    <ipxact:instanceName>gap9_core</ipxact:instanceName>
    <ipxact:componentRef vendor="POLITO" library="GAP9" name="core" version="1.0"/>
</ipxact:componentInstance>
```

An example of connections:

```XML
<ipxact:adHocConnection>
    <ipxact:name>core_I_to_core_converter_I_out</ipxact:name>
    <ipxact:portReferences>
    <ipxact:internalPortReference componentRef="core" portRef="I"/>
    <ipxact:internalPortReference componentRef="core_converter" portRef="I_out"/>
    </ipxact:portReferences>
</ipxact:adHocConnection>
```

The folder with the design has the required components which can be identified by their name, version, library and vendor. In the following parts we will describe different components and the decision which led to their final IPXACT form.

## Component parameters

Component parameters usually can be either simple or have a complex tree structure, such as these aprameters of the pv module (converter will be discussed later):

```JSON
    "harvester_type" :"source",
    "vref" :{
        "input_file":"input_files/gmonths.txt",
        "lut":{
            "input_values":[250,500,750,1000],
            "voltage_values":[2.9367,3.0710,3.1334,3.1689]
        }
    },
    "iref":{
        "input_file":"input_files/gmonths.txt",
        "lut":{
            "input_values":[250,500,750,1000],
            "current_values":[13.4031,27.2251,41.8848,56.7245]
        }
    },
```

Parameters like **harvester_type** will be put in the module parameters section and have a following description:

```XML
<ipxact:moduleParameters>
    <ipxact:moduleParameter parameterId="harvester_type" resolve="user" type="string">
        <ipxact:name>harvester_type</ipxact:name>
        <ipxact:value>source</ipxact:value>
    </ipxact:moduleParameter>
</ipxact:moduleParameters>
```

Parameters like **vref** and **iref** for pv modules or **states** for sensors will be put in the VendorExtensions category of the IPXACT standard:

```XML
<ipxact:vendorExtensions>
    <vref>
        <input_file>input_files/gmonths.txt</input_file>
        <lut>
            <input_values>[250,500,750,1000]</input_values>
            <voltage_values>[2.9367,3.0710,3.1334,3.1689]</voltage_values>
        </lut>
    </vref>
    <iref>
        <input_file>input_files/gmonths.txt</input_file>
        <lut>
            <input_values>[250,500,750,1000]</input_values>
            <current_values>[13.4031,27.2251,41.8848,56.7245]</current_values>
        </lut>
    </iref>
</ipxact:vendorExtensions>
```

## Converters

Since converters in the architectural diagram are presented as separate entities we decided to represent them as separate components.

The ports each converter has correspond to the ports of the components it is connected to, such as this port of the pv_module:

```XML
<ipxact:port>
    <ipxact:name>I</ipxact:name>
    <ipxact:transactional>
        <ipxact:initiative>provides</ipxact:initiative>
        <ipxact:transTypeDefs>
            <ipxact:transTypeDef>
                <ipxact:typeName>double</ipxact:typeName>
                <ipxact:viewRef>interface</ipxact:viewRef>
            </ipxact:transTypeDef>
        </ipxact:transTypeDefs>
    </ipxact:transactional>
</ipxact:port>
```

will be connected to the following port of the converter:

```XML
<ipxact:port>
    <ipxact:name>I_in</ipxact:name>
    <ipxact:transactional>
        <ipxact:initiative>requires</ipxact:initiative>
        <ipxact:transTypeDefs>
            <ipxact:transTypeDef>
                <ipxact:typeName>double</ipxact:typeName>
                <ipxact:viewRef>interface</ipxact:viewRef>
            </ipxact:transTypeDef>
        </ipxact:transTypeDefs>
    </ipxact:transactional>
</ipxact:port>
```

And for each such input port a converter will have an output port. Type transactional for the port has been chosen because the end design is written in SystemC and TLM will represent it more accurately rather than a digital port of a certain bitwidth.

## Bus

The only part where this approach doesn't fully comply with the IPXACT standard is the bus description. Normally busses have their own ipxact format to fully express the protocol, but in this project the role of the bus is less complicated, it functions as a hub with many ports and it's description in the old .json file had only 1 parameter (vref). Taking this into account we made a decision to simply put the bus description in a separate component.

Additionally, the following parameters of the system from the json file, which apply to the entire design:

```JSON
{
    "resolution":"SC_MS",

    "tracing":{
        "messy_trace":{
            "filename":"messy_trace.log",
            "resolution":{
                "unit":"SC_MS",
                "mult":1
            }
        }
    },
}
```

Will be stored as module parameters of the bus (or in it's vendor extensions for **tracing**)
