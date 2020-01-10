# Digital Logic HW 3 - Exact Boolean Minimization

###### tags: `digital_logic`

- Author: **B10630221 高昌廷**
- Date: 2019/11/09
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

<div class="page-break"></div>

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

## Algorithm

### Queen McCaskey

1. Build first column of Quine–McCluskey table by grouping 1's count in literal term
2. Try every possible combination of pairs in adjacent groups. 
3. If this pair has only 1 variable difference and the variable is not don't care, we can reduce it. 
4. Mark the terms in pair as `notPrimeImplicant`.
5. If the reduced term haven't added to Quine–McCluskey table, add it. 
6. Repeat process 2 to 5 until every possible combination of pairs in adjacent groups is checked.
7. Check every terms within same column, find the terms not marked as `notPrimeImplicant`. Add it to `primeImplicantList`.
8. Repeat process 2 to 5 on next column until it's no more possible reduction.

### Petrick’s method

1. Take `primeImplicantList` from Queen McCaskey Algorithm.
2. Build a product of sums of the rows where each row is rum-terms, and columns are product-terms.
3. Use the distributive law to turn that expression into a sum of products.
4. Choose products with fewest terms
5. Choose term or terms with fewest total literals

<div class="page-break"></div>

### Pseudo code

```c {class="line-numbers"}
QueenMcCaskey(Minterms, inputCount)
    // Build first literal term of Quine–McCluskey table
    for each minterm in Minterms
        group = popcount(minterm)
        groups[group] = groups[group] ∪ minterm
        configurations = configurations ∪ minterm

    // Build other literal term of Quine–McCluskey table
    for literal = inputCount - 1 down to 0
        for group = 0 to literal
            for each lowTerm in groups[group]
                for each highTerm in groups[group + 1]
                    if DifferenceCount(lowTerm, highTerm) = 1
                        reduced = Reduce(lowTerm, highTerm)
                        lowTerm.notPrimeImplicant = true
                        highTerm.notPrimeImplicant = true
                        if InSet(configurations, reduced)
                            continue
                        configurations = configurations ∪ reduced
                        nextGroups[group] = newGroups[group] ∪ reduced
        // Find prime implicant
        for each group in groups
            for each term in group
                if term.notPrimeImplicant = false
                   primeImplicants = primeImplicants ∪ term
        groups = nextGroups

    // Add prime implicants in last literal
    for each group in groups
        for each term in group
           primeImplicants = primeImplicants ∪ term

    return primeImplicants
```

```c {class="line-numbers"}
PetricksMethod(Minterms, PrimeImplicantList)
    for each primeImplicant in PrimeImplicantList
        for each minterm in primeImplicant.terms
            row[minterm] = row[minterm] ∪ primeImplicant.name;

    // Use the distributive law to turn that expression into a sum of products
    for each minterm, terms in rows
        if minterm.isNotDotCare
            if resultSet is NULL
                resultSet = terms
            else
                resultSet = polynomial * terms

    // Choose products with fewest terms
    resultSet = MiniumTermCount(polynomial)
    // Choose term or terms with fewest total literals
    resultSet = MiniumTotalLiteralCount(resultSet)

    return resultSet;
```

<div class="page-break"></div>

## Results

### 5-Variables example

$$\begin{aligned}
var5 = \sum & m(4,7,9,13,17,19,20,28) \\
 & + d(0,1,2,3,5,6,8,10,11,14,16,21,23,25,30,31) \\
\end{aligned}$$

```
===================================
Prime Implicants
-----------------------------------
name    group   minterms                literal_term
A       4-2     m20,28                  1-100
B       4-3     m28,30                  111-0
C       4-3     m14,30                  -1110 (DC)
D       4-4     m23,31                  1-111 (DC)
E       4-4     m30,31                  1111- (DC)
F       3-1     m1,9,17,25              --001
G       3-1     m1,5,9,13               0--01
H       3-1     m2,6,10,14              0--10 (DC)
I       2-0     m0,1,2,3,4,5,6,7        00---
J       2-0     m0,1,4,5,16,17,20,21    -0-0-
K       2-0     m0,1,2,3,8,9,10,11      0-0--
L       2-1     m1,3,5,7,17,19,21,23    -0--1
===================================
Petrick's Method Result Sum-terms
-----------------------------------
term    literal  prime_implicants
4       11       A.G.I.L
4       11       A.G.J.L
4       11       B.G.J.L
===================================
Petrick's Method Solutions
-----------------------------------
literal  prime_implicants  boolen_exp
11       A.G.I.L           acd'e' + a'd'e + a'b' + b'e
11       A.G.J.L           acd'e' + a'd'e + b'd' + b'e
11       B.G.J.L           abce' + a'd'e + b'd' + b'e
```

$$\begin{aligned}
var5 =& acd'e' + a'd'e + a'b' + b'e \\
var5 =& acd'e' + a'd'e + b'd' + b'e \\
var5 =& abce' + a'd'e + b'd' + b'e \\
\end{aligned}$$

<div class="page-break"></div>

#### PLA File

Source: `example/var5.pla`

``` {class="line-numbers"}
.i 5
.o 1
.ilb a b c d e
.ob var5
.p 24
00100 1
00111 1
01001 1
01101 1
10001 1
10011 1
10100 1
11100 1
00000 -
00001 -
00010 -
00011 -
00101 -
00110 -
01000 -
01010 -
01011 -
01110 -
10000 -
10101 -
10111 -
11001 -
11110 -
11111 -
.e
```

#### Output PLA

$$\begin{aligned}
var5 =& acd'e' + a'd'e + a'b' + b'e \\
\end{aligned}
$$


``` {class="line-numbers"}
.i 5
.o 1
.ilb a b c d e
.ob var5
.p 4
1-100 1
0--01 1
00--- 1
-0--1 1
.e
```

<div class="page-break"></div>

### 6-Variables example

$$\begin{aligned}
var6 = \sum & m(8,29,34,35,38,42,44,54) \\
 & + d(0,5,7,12,15,18,20,23,28,43,45,47,48,52,53,59) \\
\end{aligned}$$

```
===================================
Prime Implicants
-----------------------------------
name    group   minterms        literal_term
A       6-2     m18             010010 (DC)
B       5-0     m0,8            00-000
C       5-1     m8,12           001-00
D       5-2     m34,38          100-10
E       5-2     m5,7            0001-1 (DC)
F       5-2     m12,44          -01100
G       5-2     m12,28          0-1100 (DC)
H       5-2     m20,28          01-100 (DC)
I       5-2     m20,52          -10100 (DC)
J       5-2     m48,52          110-00 (DC)
K       5-3     m38,54          1-0110
L       5-3     m44,45          10110-
M       5-3     m7,15           00-111 (DC)
N       5-3     m7,23           0-0111 (DC)
O       5-3     m28,29          01110-
P       5-3     m52,54          1101-0
Q       5-3     m52,53          11010- (DC)
R       5-4     m15,47          -01111 (DC)
S       5-4     m43,47          101-11 (DC)
T       5-4     m43,59          1-1011 (DC)
U       5-4     m45,47          1011-1 (DC)
V       4-2     m34,35,42,43    10-01-
===================================
Petrick's Method Result Sum-terms
-----------------------------------
term    literal  prime_implicants
5       24      B.F.K.O.V
5       24      C.F.K.O.V
5       24      B.K.L.O.V
5       24      C.K.L.O.V
6       29      B.D.F.O.P.V
6       29      C.D.F.O.P.V
6       29      B.D.L.O.P.V
6       29      C.D.L.O.P.V
===================================
Petrick's Method Solutions
-----------------------------------
literal  prime_implicants  boolen_exp
24       B.F.K.O.V         a'b'd'e'f' + b'cde'f' + ac'def' + a'bcde' + ab'd'e
24       C.F.K.O.V         a'b'ce'f' + b'cde'f' + ac'def' + a'bcde' + ab'd'e
24       B.K.L.O.V         a'b'd'e'f' + ac'def' + ab'cde' + a'bcde' + ab'd'e
24       C.K.L.O.V         a'b'ce'f' + ac'def' + ab'cde' + a'bcde' + ab'd'e
```

$$\begin{aligned}
var6 =& a'b'd'e'f' + b'cde'f' + ac'def' + a'bcde' + ab'd'e \\
var6 =& a'b'ce'f' + b'cde'f' + ac'def' + a'bcde' + ab'd'e \\
var6 =& a'b'd'e'f' + ac'def' + ab'cde' + a'bcde' + ab'd'e \\
var6 =& a'b'ce'f' + ac'def' + ab'cde' + a'bcde' + ab'd'e \\
\end{aligned}$$

<div class="page-break"></div>

#### PLA File

Source: `example/var6.pla`

``` {class="line-numbers"}
.i 6
.o 1
.ilb a b c d e f
.ob var6
.p 24
000000 -
000101 -
000111 -
001100 -
001111 -
010010 -
010100 -
010111 -
011100 -
101011 -
101101 -
101111 -
110000 -
110100 -
110101 -
111011 -
001000 1
011101 1
100010 1
100011 1
100110 1
101010 1
101100 1
110110 1
.e
```

#### Output PLA

$$\begin{aligned}
var6 =& a'b'd'e'f' + b'cde'f' + ac'def' + a'bcde' + ab'd'e \\
\end{aligned}$$

```
.i 6
.o 1
.ilb a b c d e f
.ob var6
.p 5
00-000 1
-01100 1
1-0110 1
01110- 1
10-01- 1
.e
```

<div class="page-break"></div>

### 7-Variables example

$$\begin{aligned}
var7 = \sum & m(5,9,10,22,35,40,54,67,73,91,93,94,95,97,113,121) \\
 & + d(3,12,27,32,45,69,79,82,87,92,98,99,108,112,126,127) \\
\end{aligned}$$

```
===================================
Prime Implicants
-----------------------------------
name    group   minterms        literal_term
A       7-2     m10             0001010
B       7-2     m12             0001100 (DC)
C       7-3     m82             1010010 (DC)
D       7-4     m45             0101101 (DC)
E       7-4     m108            1101100 (DC)
F       6-1     m32,40          010-000
G       6-2     m5,69           -000101
H       6-2     m9,73           -001001
I       6-3     m22,54          0-10110
J       6-3     m97,113         11-0001
K       6-3     m97,99          11000-1
L       6-3     m98,99          110001- (DC)
M       6-3     m112,113        111000-
N       6-4     m113,121        111-001
O       6-4     m27,91          -011011
P       6-5     m91,95          1011-11
Q       6-5     m79,95          10-1111
R       6-5     m87,95          101-111
S       5-2     m3,35,67,99     --00011
T       5-4     m92,93,94,95    10111--
U       5-5     m94,95,126,127  1-1111-
===================================
Petrick's Method Result Sum-terms
-----------------------------------
term    literal  prime_implicants
10      59       A.F.G.H.I.J.N.P.S.T
10      59       A.F.G.H.I.J.N.O.S.T
10      59       A.F.G.H.I.K.N.P.S.T
10      59       A.F.G.H.I.K.N.O.S.T
===================================
Petrick's Method Solutions
-----------------------------------
literal  prime_implicants        boolen_exp
59       A.F.G.H.I.J.N.P.S.T     a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' + abd'e'f'g
                                 + abce'f'g + ab'cdfg + c'd'e'fg + ab'cde 
59       A.F.G.H.I.J.N.O.S.T     a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' + abd'e'f'g
                                 + abce'f'g + b'cde'fg + c'd'e'fg + ab'cde
59       A.F.G.H.I.K.N.P.S.T     a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' + abc'd'e'g
                                 + abce'f'g + ab'cdfg + c'd'e'fg + ab'cde 
59       A.F.G.H.I.K.N.O.S.T     a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' + abc'd'e'g
                                 + abce'f'g + b'cde'fg + c'd'e'fg + ab'cde
```

$$\begin{aligned}
var7 =& a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' \\
&+ abd'e'f'g + abce'f'g + ab'cdfg + c'd'e'fg + ab'cde \\
var7 =& a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' \\
&+ abd'e'f'g + abce'f'g + b'cde'fg + c'd'e'fg + ab'cde \\
var7 =& a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' \\
&+ abc'd'e'g + abce'f'g + ab'cdfg + c'd'e'fg + ab'cde \\
var7 =& a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' \\
&+ abc'd'e'g + abce'f'g + b'cde'fg + c'd'e'fg + ab'cde \\
\end{aligned}$$

<div class="page-break"></div>

#### PLA File

Source: `example/var7.pla`

``` {class="line-numbers"}
.i 7
.o 1
.ilb a b c d e f g
.ob var7
.p 32
0000011 -
0001100 -
0011011 -
0100000 -
0101101 -
1000101 -
1001111 -
1010010 -
1010111 -
1011100 -
1100010 -
1100011 -
1101100 -
1110000 -
1111110 -
1111111 -
0000101 1
0001001 1
0001010 1
0010110 1
0100011 1
0101000 1
0110110 1
1000011 1
1001001 1
1011011 1
1011101 1
1011110 1
1011111 1
1100001 1
1110001 1
1111001 1
.e
```

#### Output PLA

$$\begin{aligned}
var7 =& a'b'c'de'fg' + a'bc'e'f'g' + b'c'd'ef'g + b'c'de'f'g + a'cd'efg' \\
&+ abd'e'f'g + abce'f'g + ab'cdfg + c'd'e'fg + ab'cde \\
\end{aligned}$$

``` {class="line-numbers"}
.i 7
.o 1
.ilb a b c d e f g
.ob var7
.p 10
0001010 1
010-000 1
-000101 1
-001001 1
0-10110 1
11-0001 1
111-001 1
1011-11 1
--00011 1
10111-- 1
.e
```
