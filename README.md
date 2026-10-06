# Stack Interpreter

A simple stack-based language created as a learning project to explore how a compiler works. The project implements stages such as lexical analysis, parsing, compilation to bytecode, and execution on a virtual machine.

## Example

```text
push 10 + 2 * 4
print
```

## Build and run

This project uses CMake. From the repository root:

```sh
cmake -S . -B build
cmake --build build
./build/Stack-Interpreter src/source.txt
```