# Virtual Machine in C

This project implements a simple virtual machine that simulates a CPU with registers and memory (RAM). The machine reads and executes assembly-like instructions from an input file, manipulating registers and RAM and storing its state in a database file (`db.txt`).

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Requirements](#requirements)
- [Compilation and Execution](#compilation-and-execution)
- [Assembly Language](#assembly-language)
- [Instruction Set](#instruction-set)
- [File Structure](#file-structure)
- [Detailed Code Description](#detailed-code-description)

## Overview

The virtual machine simulates a simple CPU with:
- **9 registers**.
- **256 bytes of RAM**.
- It supports basic arithmetic, load/store, and control instructions.
  
The state of the registers and memory can be saved to a file, allowing for persistence across runs. This project aims to simulate the basic operations of a CPU using C.

## Features

- **Register Operations**: Move values between registers, perform addition and subtraction.
- **Memory Operations**: Load values from and store values to specific memory addresses in RAM.
- **Instruction Execution**: Read an assembly-like instruction file and execute the instructions sequentially.
- **Database**: Save and load the state of registers and memory to/from a file.
  
## Requirements

To compile and run the project, you will need:

- GCC or any C compiler.
- A Unix-based system for POSIX file handling (or adjust for other platforms).
  
## Compilation and Execution

### 1. Compilation

To compile the project, run:

```bash
gcc -o vm main.c ram.c registers.c instructions.c
