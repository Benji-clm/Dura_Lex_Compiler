# Dura Lex Sed Lex - Compilers Coursework (2024/25)

This repository contains the coursework for the 2024/25 Compilers module at Imperial College London.

## Project Overview

This project involves building a compiler for a simplified programming language. Key tasks include:

- Lexical analysis
- Parsing
- Abstract Syntax Tree (AST) generation
- Semantic analysis
- Code generation and optimization

The goal is to transform source code written in a custom language into working RISC-V assembly.

## Structure

```

.
├── src/                  # Source files for the compiler
├── include/              # Header files
├── compiler\_tests/       # Provided test cases
├── docs/                 # Documentation
├── scripts/              # Useful helper scripts
├── Makefile              # Build configuration
└── test.sh               # Automated test script

````

## Running Tests

For the development we used an Ubuntu 22.04 Dockerfile, containing all the needed tools for testing and compiling. If you want to test the compiler, clone this repo and re-open it in the Docker Container.

Then you can simply run:

```bash
./test.sh
```

or

```
scripts/test.py
```

**Note**: I have modified the Makefile and the Python code to enable multithreading and increase the number of jobs in the queue. This significantly improves testing speed, but may cause tests to fail intermittently due to out-of-order execution.

## Acknowledgements

[godbolt](https://godbolt.org/) was an absolute really helpful ino having an insight into different levels of optimisations when doing the code generation.

[port70.net](https://port70.net/~nsz/c/c99/n1256.html) for providing C documnetations to help understand how C works.

Special thanks to [saturn691](https://github.com/saturn691) for giving me guidance and tips to write the compiler.