import argparse
import subprocess
from pathlib import Path
from typing import List

def run_messy(config_filename: str=None, application: str=None, skip: List[str]=[]):
    if "clean" not in skip:
        subprocess.run(["make","clean"])
    if "codegen" not in skip:
        subprocess.run(["make","codegen",f"file={Path(config_filename).absolute()}"])
    if "application" not in skip:
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

    parser.add_argument(
        "--skip_clean",
        action="store_true",
        help="Skip the cleaning of the current build",
    )

    parser.add_argument(
        "--skip_codegen",
        action="store_true",
        help="Skip the codegen part of the current build",
    )

    parser.add_argument(
        "--skip_application",
        action="store_true",
        help="Skip the application regeneration part of the current build",
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

    skips=[]
    if args.skip_application:
        skips.append("application")
    if args.skip_codegen:
        skips.append("codegen")
    if args.skip_clean:
        skips.append("clean")
    run_messy(config_filename=config_filename,application=application,skip=skips)