# Documentation
Until the project is not public, you can read the documentation in the following way, supposing you are using Visual Studio Code:

Clone the repository

```bash
git clone https://github.com/eml-eda/messy
```

Change into the directory

```bash
cd messy
```
Open this folder in Visual Studio Code

Create a python virtual environment

```bash
python3 -m venv doc_env
```

Activate the virtual environment

```bash
source doc_env/bin/activate
```

Install the dependencies

```bash
pip3 install -r requirements.txt
```

Serve the documentation

```bash
mkdocs serve
```

Now you see something like this:

```
Serving on http://127.0.0.1:8000/
```

Visual Studio Code should automatically setup port forwarding. So you can simply open a browser and go to [http://127.0.0.1:8000](http://127.0.0.1:8000) to see the documentation.

