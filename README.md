# Hash Table Encoding System

## Overview
This project implements a chaining-based hash table in C++ for storing string tokens read from standard input. The hash function groups tokens by their first letter, and collisions are handled using linked lists.

## Features
- Custom hash function based on the first character of each token
- Collision handling through separate chaining
- Prints the contents of the first 5 hash table slots
- Prints the length of each slot
- Computes the standard deviation of slot lengths

## Files
- `main.cpp` — input handling and program flow
- `hash.cpp` — hash table implementation
- `hash.h` — declarations for the hash table and linked-list node
- `Makefile` — build instructions

## Build
```bash
make
