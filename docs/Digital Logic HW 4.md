# Digital Logic HW 4 - State Minimization

###### tags: `digital_logic`

- Author: **B10630221 高昌廷**
- Date: 2019/12/10
- OS: Ubuntu 18.04
- Compiler: g++

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

```shell {class="line-numbers"}
sudo apt-get install build-essential
```

### Install graphviz

```shell {class="line-numbers"}
sudo apt-get install graphviz
```

### Build

```shell {class="line-numbers"}
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

```shell {class="line-numbers"}
./smin input.kiss output.kiss output.dot
```

And convert from dot to png image

```shell {class="line-numbers"}
dot -T png output.dot > output.png
```

All the above can also be combine in one line with `-m` option.
> -m    Output image as well. ('dot' binary must be installed first)

```shell {class="line-numbers"}
./smin -m input.kiss output.kiss output.dot
```

The above will output `output.dot` and `output.dot.png`.

### Output original stg.dot

Use `-o` option.
> -o    Output stg.dot as well.

```shell {class="line-numbers"}
./smin -o input.kiss output.kiss output.dot
```

The above will output `output.dot`, `output.kiss` and `output_stg.dot`.

This can be combined with `-m` to output obdd image as well.

```shell {class="line-numbers"}
./smin -o -m input.kiss output.kiss output.dot
```

The above will output `output.dot`, `output.kiss`,  `output.dot.png`, `output_stg.dot`, `output_stg.png`.

### Output reduction process

You can output the reduction process with `-v` option.

> -v    Showing all debug information.

```shell {class="line-numbers"}
./smin -v input.kiss output.kiss output.dot
```

This option can also be combined with `-m` and `-o` options like below:

```shell {class="line-numbers"}
./smin -o -m -v input.kiss output.kiss output.dot
```

## Simplification algorithm

<div class="page-break"></div>

### Pseudo code

```cpp {class="line-numbers"}
Minimize(nodes)
    // Step 1: List transition (next state) pairs.
    for FROM from 0 to stateCount
        for TO from 0 to FROM
            if FROM = TO 
                implicationTable[FROM][TO].isValid = true
                continue

            allSame = true
            for i from 0 to 2^inputCount
                if nodes[FROM][i].value != nodes[TO][i].value)
                    allSame = false
                    break

            if allSame 
                implicationTable[FROM][TO].isValid = true
                for i from 0 to 2^inputCount
                    implicationTable[FROM][TO].pairs = 
                         implicationTable[FROM][TO].pairs ∪ (nodes[FROM][i].state, nodes[to][i].state)

            else 
                implicationTable[FROM][TO].isValid = false

    // Step2: Check compatibility of transition pair.
    do
        hasChanged = false
        for FROM from 0 to stateCount
            for TO from 0 to FROM
                if not implicationTable[FROM][TO].isValid
                    continue

                for i from 0 to 2^inputCount
                    s1 = implicationTable[FROM][TO].pairs[i][0]
                    s2 = implicationTable[FROM][TO].pairs[i][1]
                    if s1 < s2
                        swap(s1, s2)

                    if not implicationTable[s1][s2].isValid
                        // Remove incompatible transition pairs
                        implicationTable[FROM][TO].isValid = false
                        hasChanged = true
                        break         
    while (hasChanged)

    // Step3: Merge remaining compatible states.
    for FROM from 0 to stateCount
        for TO from 0 to FROM
            if not implicationTable[from][to].isValid
                continue

            f = FROM
            t = TO
            if f < t
                swap(f, t)

            // Update implication table
            for i from 0 to stateCount
                implicationTable[f][i].isValid = false
                implicationTable[i][f].isValid = false

            // Remove state
            nodes.remove(f)
            // Replace state
            for i from 0 to nodes.length
                for j from 0 to stateCount
                    if nodes[i][j].state = f
                        nodes[i][j].state = t
```

<div class="page-break"></div>

## Result

### Case 1 (4 states, 1 input)

Source: `tests/case1.kiss`

#### Input

``` {class="line-numbers"}
.start_kiss
.i 1
.o 1
.p 8
.s 4
.r a
0 a b 0
1 a b 0
0 b c 0
1 b a 1
0 c c 0
1 c d 0
0 d c 0
1 d a 1
.end_kiss
```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    a [label="a"];
    b [label="b"];
    c [label="c"];
    d [label="d"];

    INIT -> a;
    a -> b [label="0/0,1/0"];
    b -> c [label="0/0"];
    b -> a [label="1/1"];
    c -> c [label="0/0"];
    c -> d [label="1/0"];
    d -> c [label="0/0"];
    d -> a [label="1/1"];
}
```

![](https://i.imgur.com/Zhhdy72.png)

<div class="page-break"></div>

#### Output

``` {class="line-numbers"}
.start_kiss
.i 1
.o 1
.p 6
.s 3
.r a
0 a b 0
1 a b 0
0 b c 0
1 b a 1
0 c c 0
1 c b 0
.end_kiss
```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    a [label="a"];
    b [label="b"];
    c [label="c"];

    INIT -> a;
    a -> b [label="0/0,1/0"];
    b -> c [label="0/0"];
    b -> a [label="1/1"];
    c -> c [label="0/0"];
    c -> b [label="1/0"];
}
```

![](https://i.imgur.com/kyzmqjA.png)

<div class="page-break"></div>

#### Process

```
 -
 -

c-b  -
d-b  -

 -  c-c  -
 -  a-a  -
===================================
 -  
 -

 -   -
 -   -  

 -  c-c  -
 -  a-a  -
===================================
 -
 -

 -   -
 -   -

 -  c-c  -
 -  a-a  -
===================================
  | in 0| in 1|
a | b 0 | b 0 |
b | c 0 | a 1 |
c | c 0 | d 0 |
d | c 0 | a 1 |
===================================
  | in 0| in 1|
a | b 0 | b 0 |
b | c 0 | a 1 |
c | c 0 | b 0 |
===================================
```

<div class="page-break"></div>

### Case 2 (6 states, 1 input)

Source: `tests/case2.kiss`

#### Input

``` {class="line-numbers"}
.start_kiss
.i 1
.o 1
.p 12
.s 6
.r a
0 a d 0
1 a c 0
0 b a 0
1 b e 0
0 c a 0
1 c e 0
0 d e 0
1 d e 1
0 e a 0
1 e c 0
0 f f 0
1 f a 0
.end_kiss
```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    a [label="a"];
    b [label="b"];
    c [label="c"];
    d [label="d"];
    e [label="e"];
    f [label="f"];

    INIT -> a;
    a -> d [label="0/0"];
    a -> c [label="1/0"];
    b -> a [label="0/0"];
    b -> e [label="1/0"];
    c -> a [label="0/0"];
    c -> e [label="1/0"];
    d -> e [label="0/0,1/1"];
    e -> a [label="0/0"];
    e -> c [label="1/0"];
    f -> f [label="0/0"];
    f -> a [label="1/0"];
}
```
![](https://i.imgur.com/8MuTdAo.png)

<div class="page-break"></div>

#### Output

``` {class="line-numbers"}
.start_kiss
.i 1
.o 1
.p 8
.s 4
.r a
0 a d 0
1 a b 0
0 b a 0
1 b b 0
0 d b 0
1 d b 1
0 f f 0
1 f a 0
.end_kiss

```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    a [label="a"];
    b [label="b"];
    d [label="d"];
    f [label="f"];

    INIT -> a;
    a -> d [label="0/0"];
    a -> b [label="1/0"];
    b -> a [label="0/0"];
    b -> b [label="1/0"];
    d -> b [label="0/0,1/1"];
    f -> f [label="0/0"];
    f -> a [label="1/0"];
}
```

![](https://i.imgur.com/PMCvJWz.png)


<div class="page-break"></div>

### Process

```
a-d
e-c 

a-d a-a
e-c e-e

 -   -   -
 -   -   -

a-d a-a a-a  -
c-c c-e c-e  -

f-d f-a f-a  -  f-a
a-c a-e a-e  -  a-c
===================================
 -
 -

 -  a-a
 -  e-e

 -   -   -
 -   -   -

 -  a-a a-a  -
 -  c-e c-e  -

 -   -   -   -   -
 -   -   -   -   -
===================================
 -
 -

 -  a-a 
 -  e-e 

 -   -   -  
 -   -   -

 -  a-a a-a  -
 -  c-e c-e  -

 -   -   -   -   -
 -   -   -   -   -
===================================
  | in 0| in 1|
a | d 0 | c 0 |
b | a 0 | e 0 |
c | a 0 | e 0 |
d | e 0 | e 1 |
e | a 0 | c 0 |
f | f 0 | a 0 |
===================================
  | in 0| in 1|
a | d 0 | b 0 |
b | a 0 | e 0 |
d | e 0 | e 1 |
e | a 0 | b 0 |
f | f 0 | a 0 |
===================================
  | in 0| in 1|
a | d 0 | b 0 |
b | a 0 | b 0 |
d | b 0 | b 1 |
f | f 0 | a 0 |
===================================
```

<div class="page-break"></div>

### Case 3 (6 states, 2 inputs)

Source: `tests/case3.kiss`

#### Input

``` {class="line-numbers"}
.start_kiss
.i 2
.o 1
.p 24
.s 6
.r S1
00 S1 S6 0
01 S1 S5 0
10 S1 S4 0
11 S1 S3 0
00 S2 S2 0
01 S2 S1 0
10 S2 S6 0
11 S2 S5 0
00 S3 S4 0
01 S3 S3 0
10 S3 S2 0
11 S3 S1 0
00 S4 S6 1
01 S4 S5 0
10 S4 S4 0
11 S4 S3 0
00 S5 S2 0
01 S5 S1 0
10 S5 S6 0
11 S5 S5 0
00 S6 S4 0
01 S6 S3 0
10 S6 S2 0
11 S6 S1 0
.end_kiss
```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    S1 [label="S1"];
    S2 [label="S2"];
    S3 [label="S3"];
    S4 [label="S4"];
    S5 [label="S5"];
    S6 [label="S6"];

    INIT -> S1;
    S1 -> S6 [label="00/0"];
    S1 -> S5 [label="01/0"];
    S1 -> S4 [label="10/0"];
    S1 -> S3 [label="11/0"];
    S2 -> S2 [label="00/0"];
    S2 -> S1 [label="01/0"];
    S2 -> S6 [label="10/0"];
    S2 -> S5 [label="11/0"];
    S3 -> S4 [label="00/0"];
    S3 -> S3 [label="01/0"];
    S3 -> S2 [label="10/0"];
    S3 -> S1 [label="11/0"];
    S4 -> S6 [label="00/1"];
    S4 -> S5 [label="01/0"];
    S4 -> S4 [label="10/0"];
    S4 -> S3 [label="11/0"];
    S5 -> S2 [label="00/0"];
    S5 -> S1 [label="01/0"];
    S5 -> S6 [label="10/0"];
    S5 -> S5 [label="11/0"];
    S6 -> S4 [label="00/0"];
    S6 -> S3 [label="01/0"];
    S6 -> S2 [label="10/0"];
    S6 -> S1 [label="11/0"];
}
```
![](https://i.imgur.com/6siDOY1.png)

<div class="page-break"></div>

#### Output

``` {class="line-numbers"}
.start_kiss
.i 2
.o 1
.p 16
.s 4
.r S1
00 S1 S3 0
01 S1 S2 0
10 S1 S4 0
11 S1 S3 0
00 S2 S2 0
01 S2 S1 0
10 S2 S3 0
11 S2 S2 0
00 S3 S4 0
01 S3 S3 0
10 S3 S2 0
11 S3 S1 0
00 S4 S3 1
01 S4 S2 0
10 S4 S4 0
11 S4 S3 0
.end_kiss
```

``` {class="line-numbers"}
digraph STG {
    rankdir=LR;

    INIT [shape=point];
    S1 [label="S1"];
    S2 [label="S2"];
    S3 [label="S3"];
    S4 [label="S4"];

    INIT -> S1;
    S1 -> S3 [label="00/0,11/0"];
    S1 -> S2 [label="01/0"];
    S1 -> S4 [label="10/0"];
    S2 -> S2 [label="00/0,11/0"];
    S2 -> S1 [label="01/0"];
    S2 -> S3 [label="10/0"];
    S3 -> S4 [label="00/0"];
    S3 -> S3 [label="01/0"];
    S3 -> S2 [label="10/0"];
    S3 -> S1 [label="11/0"];
    S4 -> S3 [label="00/1,11/0"];
    S4 -> S2 [label="01/0"];
    S4 -> S4 [label="10/0"];
}
```
![](https://i.imgur.com/ZzDIB4P.png)

<div class="page-break"></div>

#### Process

```
S2-S6
S1-S5
S6-S4
S5-S3

S4-S6 S4-S2
S3-S5 S3-S1
S2-S4 S2-S6
S1-S3 S1-S5

 -   -   -
 -   -   -
 -   -   -
 -   -   -  

S2-S6 S2-S2 S2-S4  -
S1-S5 S1-S1 S1-S3  -
S6-S4 S6-S6 S6-S2  -  
S5-S3 S5-S5 S5-S1  -

S4-S6 S4-S2 S4-S4  -  S4-S2
S3-S5 S3-S1 S3-S3  -  S3-S1
S2-S4 S2-S6 S2-S2  -  S2-S6
S1-S3 S1-S5 S1-S1  -  S1-S5
===================================
 -
 -
 -
 -

 -   -
 -   -
 -   -
 -   -

 -   -   -
 -   -   -
 -   -   -  
 -   -   -

 -  S2-S2  -   -
 -  S1-S1  -   -
 -  S6-S6  -   -
 -  S5-S5  -   -

 -   -  S4-S4  -   -
 -   -  S3-S3  -   -
 -   -  S2-S2  -   -
 -   -  S1-S1  -   -  
===================================
 -
 -
 -
 -

 -   -
 -   -
 -   -
 -   -

 -   -   -
 -   -   -  
 -   -   -
 -   -   -

 -  S2-S2  -   -
 -  S1-S1  -   -  
 -  S6-S6  -   -
 -  S5-S5  -   -

 -   -  S4-S4  -   -
 -   -  S3-S3  -   -
 -   -  S2-S2  -   -
 -   -  S1-S1  -   -
===================================
   | in 00| in 01| in 10| in 11|
S1 | S6 0 | S5 0 | S4 0 | S3 0 |
S2 | S2 0 | S1 0 | S6 0 | S5 0 |
S3 | S4 0 | S3 0 | S2 0 | S1 0 |
S4 | S6 1 | S5 0 | S4 0 | S3 0 |
S5 | S2 0 | S1 0 | S6 0 | S5 0 |
S6 | S4 0 | S3 0 | S2 0 | S1 0 |
===================================
   | in 00| in 01| in 10| in 11|
S1 | S6 0 | S2 0 | S4 0 | S3 0 |
S2 | S2 0 | S1 0 | S6 0 | S2 0 |
S3 | S4 0 | S3 0 | S2 0 | S1 0 |
S4 | S6 1 | S2 0 | S4 0 | S3 0 |
S6 | S4 0 | S3 0 | S2 0 | S1 0 |
===================================
   | in 00| in 01| in 10| in 11|
S1 | S3 0 | S2 0 | S4 0 | S3 0 |
S2 | S2 0 | S1 0 | S3 0 | S2 0 |
S3 | S4 0 | S3 0 | S2 0 | S1 0 |
S4 | S3 1 | S2 0 | S4 0 | S3 0 |
===================================
```