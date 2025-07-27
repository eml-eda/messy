# Usage

This document provides instructions on how to use the Messy toolchain for simulation and analysis, and how to build and serve the project documentation.

## Running Simulations

The primary way to interact with the Messy toolchain is through the `messy.py` script, which orchestrates the `Makefile` targets.

To build and run a simulation:

```bash
python3 messy/messy.py -f <path_to_config_file> -a <path_to_application>
```

*   `<path_to_config_file>`: Path to a JSON configuration file (e.g., `messy/codegen/pulp_open.json`) or an IP-XACT XML file (e.g., `messy/codegen/ipxact/pulp_open_pv_panel/pulp_open_design.xml`). This file defines the system's components and their parameters.
*   `<path_to_application>`: Path to the embedded application source code (e.g., `examples/helloworld`).

**Example:**

```bash
python3 messy/messy.py -f messy/codegen/pulp_open.json -a examples/helloworld
```

### Skipping Steps

You can skip certain steps in the `messy.py` workflow using command-line flags:

*   `--skip_clean`: Skips the `make clean` step.
*   `--skip_codegen`: Skips the code generation (`make codegen` or `make codegen_ipxact`) and formatting steps.
*   `--skip_application`: Skips the application building (`make application`) step.
*   `--skip_docs`: Skips the documentation generation step.

## Building and Serving Documentation

To build and serve the project documentation locally, follow these steps:

### Prerequisites

Ensure you have Python 3 and `pip` installed.

### Installation

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/eml-eda/messy
    cd messy
    ```

2.  **Create a virtual environment (recommended):**

    ```bash
    python3 -m venv doc_env
    source doc_env/bin/activate
    ```

3.  **Install Python dependencies:**

    ```bash
    pip install -r requirements.txt
    ```

### Building and Serving

1.  **Build the documentation:**

    ```bash
    make docs
    ```

2.  **Serve the documentation locally:**

    ```bash
    mkdocs serve
    ```

    This will typically serve the documentation at `http://127.0.0.1:8000/`. Open this URL in your web browser to view the documentation.

