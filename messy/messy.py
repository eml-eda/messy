import argparse
import subprocess
from pathlib import Path

def run_messy(config_filename: str=None, application: str=None):
    subprocess.run(["make","clean"])
    subprocess.run(["make","codegen",f"file={Path(config_filename).absolute()}"])
    subprocess.run(["make","application",f"app={Path(application).absolute()}"])
    subprocess.run(["make","run"])


if __name__ == "__main__":
    # Create an argument parser with a description
    parser = argparse.ArgumentParser(description="Generate src code")

    # Add an argument for the input file path
    parser.add_argument(
        "-f",
        "--filename",
        type=str,
        default="codegen/pulp-open.json",
        metavar="PATH",
        help="Path to JSON input simulation settings",
    )

    parser.add_argument(
        "-a",
        "--application",
        type=str,
        default="/messy/examples/helloworld",
        metavar="PATH",
        help="Path to the application to simulate",
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

    run_messy(config_filename=config_filename,application=application)