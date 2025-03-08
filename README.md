# Pattern Math: Symbolic Dynamics and Cryptographic Hashing

This repository contains a research paper and associated source code exploring an abstract mathematical system inspired by symbolic dynamics and rewriting systems. The system defines a set of symbols and rewriting rules with applications in computation and cryptography, including a fixed-point theorem and a custom hash function implementation.

## Repository Structure
```
/pattern-math
├── a.out                  # Compiled executable of the hash function
├── pattern-hashing.cpp    # C++ source code with inline assembly implementing the hash function
├── pattern-hash.asm       # Standalone assembly version of the hash function
├── pattern-learning.tex   # Main LaTeX source for the research paper
├── pattern-learning-def.tex # Supplementary LaTeX definitions (if applicable)
├── pattern-learning.aux   # LaTeX auxiliary file (generated)
├── pattern-learning.log   # LaTeX compilation log file (generated)
├── pattern-learning.out   # LaTeX output file (generated)
├── pattern-learning.pdf   # Final compiled research paper (PDF)
├── pattern-learning.tex~  # Backup file for the LaTeX source
```

## Overview

The research paper, "Mathematical Object Definition and Its Applications", outlines:

### System Definition
The paper introduces a symbolic system operating over the set  
\(\mathcal{S} = \{*, \&, 1, x\}\)  
with non-deterministic rewriting rules, such as:  
- \( * \rightarrow 1 \)  
- \( \& \rightarrow *1 \)  
- \( * + 1 \rightarrow \&*(1x) \)  
- \( * + 1 \rightarrow \& + 1 \)

### Length Function
A mapping \(\text{Len}\) is defined on the symbols, assigning numeric or infinite values to each.

### Fixed Point Theorem
The paper provides a theorem guaranteeing the existence of an infinite fixed point in the symbol substitution space.

### Cryptographic Application
It defines a cryptographic hash function \( H \) based on the extended length function, including a proof sketch for its preimage resistance.

### Assembly Implementation
An inline assembly implementation of the hash function is provided in the source code (`pattern-hashing.cpp`) and as a standalone file (`pattern-hash.asm`).

## Building and Running the Code

### Compiling the C++ Source
The hash function is implemented in C++ with inline assembly. To compile the code with Clang, run:

```bash
clang++ -Wall -Wextra -Wpedantic -Werror --std=c++23 -g -O0 --debug pattern-hashing.cpp -o a.out
```

This command builds the executable a.out with debugging enabled.
## Running the Executable

After compilation, execute the program:

```bash
./a.out
```

This runs the test case embedded in the source code and prints the computed hash value.

## Compiling the Research Paper

The research paper is written in LaTeX. To compile the paper and regenerate the PDF, use:

```bash
pdflatex pattern-learning.tex
```

The resulting pattern-learning.pdf details the mathematical framework and cryptographic applications discussed in the paper.

## License
MIT 

## Contact

x.com/@piotroxp

For questions or further information, please contact on X.

## Fixes and Improvements
TBD
