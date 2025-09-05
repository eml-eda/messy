SHELL := /bin/bash
.SUFFIXES:
.SUFFIXES: .cc .cpp .o
.PHONY: clean ultraclean help

# Compiler settings
CC := g++
OPT =
DEBUG = 
OTHER := -Wall -O3

# Use this CFLAGS definition if you need to see debug output
# CFLAGS = -c -g -Wno-deprecated -Wall -D_DEBUG
CFLAGS := -c -g -Wno-deprecated $(OTHER)

# The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR := src
INCDIR := include
BUILDDIR := obj
TARGETDIR := bin

# File extensions
SRCEXT := cpp
OBJEXT := o

# External library paths
SYSTEMC_HOME := /usr/local/systemc
AMS_HOME := /usr/local/systemc-ams
MODULE := run

# Find all source files
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(SOURCES:$(SRCDIR)/%.$(SRCEXT)=$(BUILDDIR)/%.$(OBJEXT))

# include Makefile.defs
INCDIR = -I./include -I./src \
		 -I$(AMS_HOME)/include \
		 -I$(SYSTEMC_HOME)/include \
		 -I$(SYSTEMC_HOME)/src/sysc/libs \
		 -I$(SYSTEMC_HOME)/src/sysc/kernel \

EXE := $(MODULE)

# Rule to compile source files into object files
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@echo "Compiling source $< into object $@"
	@mkdir -p '$(@D)'
	$(GCC_PATH)$(CC) $(CFLAGS) $(INCDIR) -std=c++14 $< -o $@

# Rule to compile source files into object files
$(TARGETDIR)/$(EXE): $(OBJECTS)
	@echo "Building $@ from $<"
	@mkdir -p '$(@D)'
	$(GCC_PATH)$(CC) -g $(INCDIR) -std=c++14 -o $@ $(OBJECTS) \
	$(AMS_HOME)/lib-linux64/libsystemc-ams.a \
	$(SYSTEMC_HOME)/lib-linux64/libsystemc.a -lpthread 2>&1 | c++filt \

# Clean target to remove build artifacts
clean:
	echo "Cleaning up"
	rm -rf $(BUILDDIR)
	rm -rf $(TARGETDIR)
	rm -rf src
	rm -rf codegen/src
	rm -rf include
	rm -rf codegen/include
	rm -rf codegen/__pycache__

# Code generation target. The target depends on the clean target to ensure that the code is regenerated
codegen: clean
	echo "Generating code"
	python3 codegen/codegen.py -f $(file)

# Rule to format all the generated code
format:
	clang-format -i src/*.cpp include/*.hpp

codegen_ipxact: clean
	echo "Generating code from an IPXACT description"
	python3 codegen/codegen_ipxact.py -f $(file)

run: $(TARGETDIR)/$(EXE)
	@echo "Running $(EXE)"
	cd /messy/messy;\
	$(TARGETDIR)/$(EXE)

# Ultra clean target to remove all generated files
ultraclean: clean
	rm -f Makefile.deps
	rm -f tx_uart.log
	rm -f trace_file.txt
	rm -f power_trace.txt
	rm -f power_report.csv
	rm -f gvsoc_config.json
	rm -f functional_trace.txt
	rm -f flash.bin
	rm -f efuse_preload.data
	rm -f chip.soc.mram.bin
	rm -rf gvsoc_deps
	rm -f all.vcd
	rm -f all.vcd.hier
	rm -f request_adresses.log
	rm -f request_sizes.log
	rm -f messy_trace.log

Makefile.deps:
	$(GCC_PATH)$(CC) $(CFLAGS) $(INCDIR) -M $(SOURCES) $(SRCS_BUS) >> Makefile.deps

application:
	@echo "Copying application binaries from $(app)"
	cd /messy/messy/;\
	mkdir -p bin;\
	cd bin;\
	cp $(app) chessy_app;

docs:
	mkdocs build

help:
	@echo "Available targets:"
	@echo "  all       		: Build the executable"
	@echo "  clean     		: Remove build and generated files"
	@echo "  codegen   		: Generate code. Usage example: make codegen file=/messy/messy/codegen/pulp_open.json"
	@echo "  ultraclean		: Perform a clean and remove additional files, such as .log, .csv and GvSoC configuration files"
	@echo "  run       		: Build the executable and run it without regenerating the code"
	@echo "  application 		: Build and run the specified application. Usage example: make application app=/messy/examples/helloworld"
	@echo "  docs      		: Build the project documentation"
	@echo "  help      		: Display this help message"
