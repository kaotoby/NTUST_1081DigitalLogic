# Digital Logic HW 3 - Exact Boolean Minimization

- Author: **B10630221 高昌廷**
- Date: 2019/11/09

## File contents

|File name                | Description                                           |
|-------------------------|-------------------------------------------------------|
|Makefile                 | Build configuration for make.                         |
|bin/mini                 | Binary file prebuilt for Ubuntu 18.04 x64.            |
|examples/                | Example pla files used in this document.              |
|include/                 | Header files for src/ folder                          |
|src/main.cpp             | Program entry point.                                  |
|src/mini.cpp             | Program entry point for sub program *mini*.           |
|src/digital_logic.cpp    | Basic data structure and functions for digital logic. | 
|src/pla_file_handler.cpp | PLA file parsing and writing logic.                   |
|src/quine_mccluskey_algorithm.cpp | Quine–McCluskey algorithm implementation.    |
|src/petricks_polynomial.cpp | Polynomial implementation for Petrick's method.    |
|*src/robdd.cpp* (HW2)    | Program entry point for sub program *robdd*.          |
|*src/binary_decision_diagram.cpp (HW2)*| Binary decision diagram generation and export. |

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
Usage: mini [-v] [-a] INPUT_FILE OUTPUT_FILE

Options:
    -v    Showing debug information.
    -a    Output all solutions.
```

### Simple usage

```shell=
./mini input.pla output.pla
```

### Output all possible solutions

Use `-a` option.
> -a    Output all solutions.

```shell=
./mini -a input.pla output.pla
```

If there's 3 solutions, output will be *output.1.pla*, *output.2.pla*, *output.3.pla* and *output.4.pla*.

### Output prime implicants and other solutions

It's possible to see the reduction process, prime implicants, Petrick’s method selection process and all other possible solutions.

Use `-v` option.
> -v    Showing all debug information.

```shell=
./mini -v input.pla output.pla
```
