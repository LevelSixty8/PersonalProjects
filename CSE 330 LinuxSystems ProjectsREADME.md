# CSE 330 Linux Systems Projects

## Overview
This folder contains a collection of systems programming projects completed for **CSE 330**.  
The projects focus on Linux kernel development, process management, virtual memory, synchronization, and cache replacement concepts.

Together, these projects demonstrate hands-on experience with:
- Linux kernel modules
- custom system calls
- process and thread synchronization
- virtual-to-physical memory translation
- cache policy design

## Included Projects

### Project 2: Kernel Module and Custom System Call
Introduces Linux kernel development by implementing:
- a parameterized kernel module
- a custom system call
- a small userspace test program to invoke the syscall and verify kernel log output

**Key topics:** kernel modules, syscall design, userspace-kernel interaction

---

### Project 3: Producer-Consumer Zombie Process Monitor
Implements a bounded-buffer producer-consumer model inside the kernel using:
- kernel threads
- semaphores
- spinlocks
- process table scanning for zombie processes associated with a target UID

**Key topics:** synchronization, concurrency, kernel threads, process monitoring

---

### Project 4: Virtual-to-Physical Address Translator
Builds a kernel module that walks page tables for a target process and determines whether a given virtual address maps to:
- a physical address
- swapped memory
- or an invalid / unmapped page

Includes a userspace test program for generating addresses from different memory regions.

**Key topics:** virtual memory, page-table traversal, process memory inspection

---

### Project 5: Device-Mapper LRU Cache Policy
Implements an LRU-based cache replacement policy for a device-mapper style cache system.

Includes:
- a baseline LRU implementation
- a bonus variation with scan detection to reduce cache pollution during sequential access patterns

**Key topics:** cache replacement, kernel data structures, locality, storage systems

## Folder Structure
CSE330-Project2-SystemCall/
CSE330-Project3-ProducerConsumer/
CSE330-Project4-MemoryManager/
CSE330-Project5-LRU/
