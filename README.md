# Digital Logic HW 4 - State Minimization

- Author: **B10630221 高昌廷**
- Date: 2019/12/10

## File contents

|File name                | Description                                           |
|-------------------------|-------------------------------------------------------|
|Makefile                 | Build configuration for make.                         |
|bin/mini                 | Binary file prebuilt for Ubuntu 18.04 x64.            |
|examples/                | Example pla files used in this document.              |
|include/                 | Header files for src/ folder                          |
|src/main.cpp             | Program entry point.                                  |
|src/digital_logic.cpp    | Basic data structure and functions for digital logic. | 
|src/smin.cpp | Program entry point for sub program *smin*.                   |
|src/state_transition_graph.cpp | State transition graph generation and export.                   |
|src/kiss_file_handler.cpp | KISS file parsing and writing logic.                   |
|*src/mini.cpp (HW3)*            | Program entry point for sub program *mini*.           |
|*src/pla_file_handler.cpp (HW3)* | PLA file parsing and writing logic.                   |
|*src/quine_mccluskey_algorithm.cpp (HW3)* | Quine–McCluskey algorithm implementation.    |
|*src/petricks_polynomial.cpp (HW3)* | Polynomial implementation for Petrick's method.    |
|*src/robdd.cpp (HW2)*    | Program entry point for sub program *robdd*.          |
|*src/binary_decision_diagram.cpp (HW2)*| Binary decision diagram generation and export. |

## How to compile

Note: Command examples below are in Ubuntu 18.04, but the source code should work on all kinds of system.

### Install build tools

```shell=
sudo apt-get install build-essential
```

### Install graphviz

```shell=
sudo apt-get install graphviz
```

### Build

```shell=
make
```

## How to use

```
Usage: smin [-v] [-o] [-m] KISS_IN KISS_OUT DOT_FILE

Options:
    -m    Output image as well. ('dot' binary must be installed first)
    -v    Showing all debug information.
    -o    Output stg.dot as well.
```

### Simple usage

```shell=
./smin input.kiss output.kiss output.dot
```

And convert from dot to png image

```shell=
dot -T png output.dot > output.png
```

All the above can also be combine in one line with `-m` option.
> -m    Output image as well. ('dot' binary must be installed first)

```shell=
./smin -m input.kiss output.kiss output.dot
```

The above will output `output.dot` and `output.dot.png`.

### Output original stg.dot

Use `-o` option.
> -o    Output stg.dot as well.

```shell=
./smin -o input.kiss output.kiss output.dot
```

The above will output `output.dot`, `output.kiss` and `output_stg.dot`.

This can be combined with `-m` to output obdd image as well.

```shell=
./smin -o -m input.kiss output.kiss output.dot
```

The above will output `output.dot`, `output.kiss`,  `output.dot.png`, `output_stg.dot`, `output_stg.png`.

### Output reduction process

You can output the reduction process with `-v` option.

> -v    Showing all debug information.

```shell=
./smin -v input.kiss output.kiss output.dot
```

This option can also be combined with `-m` and `-o` options like below:

```shell=
./smin -o -m -v input.kiss output.kiss output.dot
```
