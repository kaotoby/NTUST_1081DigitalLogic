# Digital Logic HW 2 - ROBDD GenerationAssignment

###### tags: `digital_logic`

**B10630221 高昌廷**

OS: Ubuntu 18.04
Compiler: g++

## File contents

|File name             | Description                                       |
|----------------------|---------------------------------------------------|
|Makefile              | Build configuration for make.                     |
|examples/             | Example pla files from above.                     |
|src/main.cpp          | Program entry point.                              |
|src/pla_handler.cpp   | PLA file parsing, simplifying and exporting logic.|
|include/pla_handler.h | Header file for pla_handler.cpp                   |
|bin/robdd             | Binary file prebuilt for Ubuntu 18.04 x64.        |

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

<div class="page-break"></div>

### Build

```shell=
make
```

## How to use

```
Usage: robdd [-v] [-o] PLA_FILE DOT_FILE

Options:
    -m    Output image as well. ('dot' binary must be installed first)
    -v    Showing all debug information.
    -o    Output obdd.dot as well.
```

### Simple usage

```shell
./robdd input.pla output.dot
```

And convert from dot to png image

```shell=
dot -T png output.dot > output.png
```

All the above can also be comple in one line with `-m` option.
> -m    Output image as well. ('dot' binary must be installed first)

```shell=
./robdd -m input.pla output.dot
```

The above will output `output.dot` and `output.dot.png`.

<div class="page-break"></div>

### Output original obdd.dot

Use `-o` option.
> -o    Output obdd.dot as well.

```shell=
./robdd -o input.pla output.dot
```

The above will output `output.dot` and `output_obdd.dot`.

This can be combined with `-m` to ouput obdd image as well.

```shell=
./robdd -o -m input.pla output.dot
```

The above will output `output.dot`, `output.dot.png`,`output_obdd.dot`, `output_obdd.png`.

### Ouput reduction table

You can also ouput reduction table that shows in `case2.pdf` and see the reduction process.

Use `-v` option.
> -v    Showing all debug information.
> 

```shell=
./robdd -v input.pla output.dot
```

This option can also be combined with `-m` and `-o` options like below:

```shell=
./robdd -o -m -v input.pla output.dot
```

## Simplification algorithm

1. Find node `i` which has `elseEdge` equals to `thenEdge`.
2. Replace every node pointing to node `i` to the `elseEdge`.
3. Delte node `i`.
4. Repeat process 1 to 3 unitl no more to simply.
5. Find node `i` and `j` which has same `elseEdge` and `thenEdge`.
6. Replace every node pointing to node `j` to the `i`.
7. Delete node `j`.
8. Repeat process 5 to 7 unitl no more to simply.
9. Repeat process 1 to 8 unitl no more to simply.

<div class="page-break"></div>

### Pseudo code

```cpp {class="line-numbers"}
Simplify(nodes)
    do // Repeat step 1 to 8
        moreToSimplyOuter = false

        do // Repeat step 1 to 3
            moreToSimply = false
            for i = 1 to nodes.length - 1
                if nodes[i].isDeleted
                    continue

                if nodes[i].elseEdge = nodes[i].thenEdge
                    moreToSimply = true
                    moreToSimplyOuter = true
                    Replace(nodes, i, nodes[i].elseEdge)

                    nodes[i].isDeleted = true
        while (moreToSimply);

        do // Repeat step 5 to 7
            moreToSimply = false
            for i = 1 to nodes.length - 1
                if nodes[i].isDeleted
                    continue

                for j = i + 1 to nodes.length - 1
                    if nodes[j].isDeleted
                        continue

                    if nodes[i].elseEdge = nodes[j].elseEdge and
                       nodes[i].thenEdge = nodes[j].thenEdge

                        moreToSimply = true
                        moreToSimplyOuter = true
                        Replace(nodes, j, i)
                        nodes[j].isDeleted = true
        while (moreToSimply);

    while (moreToSimplyOuter);
```

```cpp {class="line-numbers"}
Replace(nodes, from, to)
    for i = 1 to nodes.length - 1
        if nodes[i].elseEdge = from
            nodes[i].elseEdge = to
        if nodes[i].thenEdge = from
            nodes[i].thenEdge = to
```

<div class="page-break"></div>

## Result

### 4-Variables example

Boolean algebra exmaple from: https://math.stackexchange.com/questions/303770/boolean-algebra-4-variable-expression-simplification

$$\begin{aligned}
var4 & = x'y'zw'+x'y'zw+xz+xyz \\
 & = xy+y'z
\end{aligned}$$

#### PLA File

Source: `example/var4.pla`

``` {class="line-numbers"}
.i 4
.o 1
.ilb x y z w
.ob var4
.p 4
0010 1
0011 1
1-1- 1
110- 1
.e
```

#### OBDD Image

<img src="https://i.imgur.com/5OzEqJf.png" width="550" />

<div class="page-break"></div>

#### ROBDD DOT

``` {class="line-numbers"}
digraph var4 {
    {rank=same 1}
    {rank=same 2 3}
    {rank=same 4}

    0 [label="0", shape=box]
    1 [label="x"]
    2 [label="y"]
    3 [label="y"]
    4 [label="z"]
    16 [label="1", shape=box]

    1 -> 2 [label="0", style=dotted]
    1 -> 3 [label="1", style=solid]
    2 -> 4 [label="0", style=dotted]
    2 -> 0 [label="1", style=solid]
    3 -> 4 [label="0", style=dotted]
    3 -> 16 [label="1", style=solid]
    4 -> 0 [label="0", style=dotted]
    4 -> 16 [label="1", style=solid]
}
```

#### ROBDD Image

![](https://i.imgur.com/3HGa7Ci.png)

<div class="page-break"></div>

### 5-Variables example

Boolean algebra example from: https://math.stackexchange.com/questions/2518012/finding-minterms-of-a-5-var-boolean-expression-having-10-terms-containing-nand

$$\begin{aligned}
var5 & = ab'e+b'c'de'+c'd'e+a'de'+a'cde'+ac'd'e+ae+ab'e'+a'c'+bcd' \\
 & = ae+ab'+a'c'+bcd'+a'de'
\end{aligned}$$

#### PLA File

Source: `example/var5.pla`

``` {class="line-numbers"}
.i 5
.o 1
.ilb a b c d e
.ob var5
.p 10
10--1 1
-0010 1
--001 1
0--10 1
0-110 1
1-001 1
1---1 1
10--0 1
0-0-- 1
-110- 1
.e
```

#### OBDD Image

![](https://i.imgur.com/WlgtUTW.png)

#### ROBDD DOT

``` {class="line-numbers"}
digraph var5 {
    {rank=same 1}
    {rank=same 2 3}
    {rank=same 4 5 6 7}
    {rank=same 8 9 10 11 12 13 14 15}
    {rank=same 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31}

    0 [label="0", shape=box]
    1 [label="a"]
    2 [label="b"]
    3 [label="b"]
    4 [label="c"]
    5 [label="c"]
    6 [label="c"]
    7 [label="c"]
    8 [label="d"]
    9 [label="d"]
    10 [label="d"]
    11 [label="d"]
    12 [label="d"]
    13 [label="d"]
    14 [label="d"]
    15 [label="d"]
    16 [label="e"]
    17 [label="e"]
    18 [label="e"]
    19 [label="e"]
    20 [label="e"]
    21 [label="e"]
    22 [label="e"]
    23 [label="e"]
    24 [label="e"]
    25 [label="e"]
    26 [label="e"]
    27 [label="e"]
    28 [label="e"]
    29 [label="e"]
    30 [label="e"]
    31 [label="e"]
    32 [label="1", shape=box]

    1 -> 2 [label="0", style=dotted]
    1 -> 3 [label="1", style=solid]
    2 -> 4 [label="0", style=dotted]
    2 -> 5 [label="1", style=solid]
    3 -> 6 [label="0", style=dotted]
    3 -> 7 [label="1", style=solid]
    4 -> 8 [label="0", style=dotted]
    4 -> 9 [label="1", style=solid]
    5 -> 10 [label="0", style=dotted]
    5 -> 11 [label="1", style=solid]
    6 -> 12 [label="0", style=dotted]
    6 -> 13 [label="1", style=solid]
    7 -> 14 [label="0", style=dotted]
    7 -> 15 [label="1", style=solid]
    8 -> 16 [label="0", style=dotted]
    8 -> 17 [label="1", style=solid]
    9 -> 18 [label="0", style=dotted]
    9 -> 19 [label="1", style=solid]
    10 -> 20 [label="0", style=dotted]
    10 -> 21 [label="1", style=solid]
    11 -> 22 [label="0", style=dotted]
    11 -> 23 [label="1", style=solid]
    12 -> 24 [label="0", style=dotted]
    12 -> 25 [label="1", style=solid]
    13 -> 26 [label="0", style=dotted]
    13 -> 27 [label="1", style=solid]
    14 -> 28 [label="0", style=dotted]
    14 -> 29 [label="1", style=solid]
    15 -> 30 [label="0", style=dotted]
    15 -> 31 [label="1", style=solid]
    16 -> 32 [label="0", style=dotted]
    16 -> 32 [label="1", style=solid]
    17 -> 32 [label="0", style=dotted]
    17 -> 32 [label="1", style=solid]
    18 -> 0 [label="0", style=dotted]
    18 -> 0 [label="1", style=solid]
    19 -> 32 [label="0", style=dotted]
    19 -> 0 [label="1", style=solid]
    20 -> 32 [label="0", style=dotted]
    20 -> 32 [label="1", style=solid]
    21 -> 32 [label="0", style=dotted]
    21 -> 32 [label="1", style=solid]
    22 -> 32 [label="0", style=dotted]
    22 -> 32 [label="1", style=solid]
    23 -> 32 [label="0", style=dotted]
    23 -> 0 [label="1", style=solid]
    24 -> 32 [label="0", style=dotted]
    24 -> 32 [label="1", style=solid]
    25 -> 32 [label="0", style=dotted]
    25 -> 32 [label="1", style=solid]
    26 -> 32 [label="0", style=dotted]
    26 -> 32 [label="1", style=solid]
    27 -> 32 [label="0", style=dotted]
    27 -> 32 [label="1", style=solid]
    28 -> 0 [label="0", style=dotted]
    28 -> 32 [label="1", style=solid]
    29 -> 0 [label="0", style=dotted]
    29 -> 32 [label="1", style=solid]
    30 -> 32 [label="0", style=dotted]
    30 -> 32 [label="1", style=solid]
    31 -> 0 [label="0", style=dotted]
    31 -> 32 [label="1", style=solid]
}
```

<div class="page-break"></div>

#### ROBDD Image

![](https://i.imgur.com/WlFwAG1.png)
