import argparse
import subprocess
from pathlib import Path
from typing import List


def run_messy(
    config_filename: Path = None, application: Path = None, skip: List[str] = [], graph: bool = False
):
    """
    Run the messy toolchain with the given configuration file and application.

    This function orchestrates the build and simulation process by calling
    various Makefile targets. It can skip certain steps based on the `skip` list.

    Args:
        config_filename: Path to the JSON input simulation settings or IP-XACT XML file.
        application: Path to the embedded application to simulate.
        skip: A list of steps to skip in the build process.
              Possible values: "clean", "codegen", "codegen_ipxact", "application", "docs".
        graph: Generate a Simulink diagram of the design.

    Returns:
        None
    """
    if "clean" not in skip:
        subprocess.run(["make", "clean"])
    if "codegen" not in skip and config_filename.suffix == ".json":
        subprocess.run(["make", "codegen", f"file={Path(config_filename).absolute()}"])
        subprocess.run(["make", "format"])
    if "codegen_ipxact" not in skip and config_filename.suffix == ".xml":
        subprocess.run(["make", "codegen_ipxact", f"file={Path(config_filename).absolute()}"])
    if "application" not in skip:
        subprocess.run(["make", "application", f"app={Path(application).absolute()}"])
    if graph:
        subprocess.run(["python3", "messy/codegen/codegen.py", "-f", str(config_filename), "--graph", "matlab/messy_diagram.m"])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Messy: A toolchain for simulation and analysis of embedded systems."
    )

    parser.add_argument(
        "-f",
        "--filename",
        type=str,
        default="codegen/pulp-open.json",
        metavar="PATH",
        help="Path to JSON input simulation settings or IP-XACT XML file.",
    )

    parser.add_argument(
        "-a",
        "--application",
        type=str,
        default="/messy/examples/helloworld",
        metavar="PATH",
        help="Path to the embedded application to simulate.",
    )

    parser.add_argument(
        "--skip_clean",
        action="store_true",
        help="Skip the cleaning of the current build artifacts.",
    )

    parser.add_argument(
        "--skip_codegen",
        action="store_true",
        help="Skip the code generation and formatting steps.",
    )

    parser.add_argument(
        "--skip_application",
        action="store_true",
        help="Skip the embedded application building step.",
    )
    
    parser.add_argument(
        "--graph",
        action="store_true",
        help="Generate a Simulink diagram of the design.",
    )

    # Parse the command line arguments
    args = parser.parse_args()

    config_filename = args.filename
    application = args.application

    # Convert the input file path to a Path object
    if args.filename:
        config_filename = Path(args.filename)
    if args.application:
        application = Path(args.application)

    skips = []
    if args.skip_application:
        skips.append("application")
    if args.skip_codegen:
        skips.append("codegen")
        skips.append("codegen_ipxact")
    if args.skip_clean:
        skips.append("clean")
    run_messy(config_filename=config_filename, application=application, skip=skips, graph=args.graph)
