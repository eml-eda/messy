from mako.template import Template
from mako import exceptions
import argparse
from pathlib import Path
import json
from typing import List


class Template_generator:
    def __init__(self, template_file, out_file, data_to_template) -> None:
        self.template_file = str(template_file)
        self.out_file = str(out_file)
        self.data_to_template = data_to_template


def generate_at_from_with(template_generators: List[Template_generator]):
    """
    Generate code from templates

    Parameters
    ----------
    template_generators : List[Template_generator]
        List of Template_generator

    Returns
    -------
    None
    """
    for template_gen in template_generators:
        print(f"Generating {template_gen.out_file}")
        template_file = Template(filename=template_gen.template_file)
        code = ""
        error_codegen = False
        try:
            code = template_file.render(**template_gen.data_to_template)
        except:
            code = exceptions.html_error_template().render()
            error_codegen = True
        with open(
            template_gen.out_file + (".html" if error_codegen else ""),
            "wb" if error_codegen else "w",
        ) as f:
            f.write(code)
        if error_codegen:
            print("ERROR WHILE GENERATING CODE!")
            break

def main(input_file, template_dir, output_dir):
    """
    Main function for code generation.

    This function reads the input settings from a JSON file, prepares the necessary
    directories, and generates SystemC/SystemC-AMS code based on the provided templates.
    It handles the configuration of core, sensors, and harvesters, including tracing
    and power conversion settings.

    Parameters
    ----------
    input_file : Path
        Path to JSON input simulation settings.
    template_dir : Path
        Path to templates directory.
    output_dir : Path
        Path where code will be generated.
    """

    # Load input settings file
    with open(input_file, "r") as f:
        settings = json.load(f)

    base_plot_dir = output_dir / "matlab"
    base_plot_dir.mkdir(parents=True, exist_ok=True)
    template_generators = []
    
    # Use default sensor templates
    template_generators.append(
        Template_generator(
            template_dir / "plotter" / "messy_diagram.m",
            base_plot_dir / "messy_diagram.m"
        )
    )

if __name__ == "__main__":
    # Create an argument parser with a description
    parser = argparse.ArgumentParser(description="Generate src code")

    # Add an argument for the input file path
    parser.add_argument(
        "-f",
        "--input-file",
        type=str,
        metavar="PATH",
        help="Path to JSON input simulation settings",
    )

    # Add an argument for the template directory path
    parser.add_argument(
        "-t",
        "--template-dir",
        type=str,
        metavar="PATH",
        default="./codegen/templates",
        help="Path to templates directory",
    )

    # Add an argument for the output directory path
    parser.add_argument(
        "-o",
        "--output-dir",
        type=str,
        metavar="PATH",
        default=".",
        help="Path where code will be generated",
    )