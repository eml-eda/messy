# IPXACT Parsing

For the purpose of working with ipxact designs instead of json designs a separate script has been created.

It utilizes functions from the `messy/codegen/ipxact.py` module

## Usage

The script that will be used is called `messy/codegen/codegen_ipxact.py` and can be used like this:

```shell
python codegen_ipxact.py -f ipxact/gap9/gap9_design.xml -d ipxact/gap9
```

The **-f** flag specifies a path to the IPXACT top-level design file. The **-d** flag specifies the folder which contains all the library modules used in the design.

## Structure

The parsing script works as follows:

1. At first the design file is read
2. Then all the components are extracted from it (function ***read_ipxact_design(file_path)***)
3. Then in for a cycle the resulting dictionary (of the same format as json) is assembled
4. For each component it's XML representation is converted to JSON (function ***xml_to_json(element) and parse_element(element)***)
5. And then it's format is adjusted to match the initial JSON format (functions ***process_bus(), process_core() etc.***)
6. For each component the version, library and vendor are checked (function ***sanity_check()***)

> #### NB!
> - Component filenaming convention is **component_name** + "_component.xml"
> - It is needed for the correct work of the script

