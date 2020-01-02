# Digital Logic HW 5 - Logic Simulation

###### tags: `digital_logic`

- Author: **B10630221 高昌廷**
- Date: 2020/01/01

## File contents

|File name                | Description                                           |
|-------------------------|-------------------------------------------------------|
|Makefile                 | Build configuration for make.                         |
|bin/mini                 | Binary file prebuilt for Ubuntu 18.04 x64.            |
|examples/                | Example pla files used in this document.              |
|include/                 | Header files for src/ folder                          |
|src/main.cpp             | Program entry point.                                  |
|src/digital_logic.cpp    | Basic data structure and functions for digital logic. |
|src/lsim.cpp                              | Program entry point for sub program *lsim*.     |
|src/logic_simulator.cpp                   | Logic circit simulator.                         |
|src/blif_file_handler.cpp                 | BLIF file parsing and writing logic.            |
|src/sti_file_handler.cpp                  | STI file parsing and writing logic.             |
|*src/smin.cpp (HW4)*                      | Program entry point for sub program *smin*.     |
|*src/state_transition_graph.cpp (HW4)*    | State transition graph generation and export.   |
|*src/kiss_file_handler.cpp (HW4)*         | KISS file parsing and writing logic.            |
|*src/mini.cpp (HW3)*                      | Program entry point for sub program *mini*.     |
|*src/pla_file_handler.cpp (HW3)*          | PLA file parsing and writing logic.             |
|*src/quine_mccluskey_algorithm.cpp (HW3)* | Quine–McCluskey algorithm implementation.       |
|*src/petricks_polynomial.cpp (HW3)*       | Polynomial implementation for Petrick's method. |
|*src/robdd.cpp (HW2)*                     | Program entry point for sub program *robdd*.    |
|*src/binary_decision_diagram.cpp (HW2)*   | Binary decision diagram generation and export.  |

## How to compile

Note: Command examples below are in Ubuntu 18.04, but the source code should work on all kinds of system.

### Install build tools

```shell=
sudo apt-get install build-essential
```

### Build

```shell=
make
```

## How to use

```
Usage: lsim [-v] PROGRAM BLIF_FILE STIMULI_FILE RESPONSE_FILE

Options:
    -v    Showing all debug information.
```

### Simple usage

```shell=
./lsim simple.blif simple.sti simple.res
```

You can also output topology sorted sequence and simulation states with `-v` option.
> -v    Showing all debug information.

```shell=
./lsim -v simple.blif simple.sti simple.res
```

The above will output `simple.res`.
