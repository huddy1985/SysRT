SysRT: A Modular Multiprocessor RTOS Simulator for Early Design Space Exploration
======================================
Authors: Jun Xiao 
Contributors: Giuseppe Lipari
Email:J.Xiao@uva.nl
This is open source software.
======================================================================
SUMMARY
1. INTRODUCTION
2. RELEASE NOTES
3. DEPENDENCIES
4. BUILDING SYSRT LIBRARY 
5. RUNNING AN EXAMPLE
======================================================================

## 1. INTRODUCTION

SysRT, a generic and modular high-level RTOS simulator that is highly suited for early DSE
to study RTOS design alternatives. The SystemC-based SysRT simulator improves on current state-of-art RTOS simulators by
providing the unique combination of being, at the same time, highly accurate, efficient and easy to extend to facilitate early
DSE. SysRT contains different types of application models, an RTOS kernel model and an abstract architecture model.

## 2. RELEASE NOTES

This software has been tested under Linux, MacOSX and Windows. Supported (tested)
compilers are:

	- g++ (Ubuntu 4.9.2-10ubuntu13) 4.9.2   

We assume the user has the compiler already installed on his machine.

## 3. DEPENDENCIES

SystemC version 2.3.1  http://www.accellera.org/.
Before you compile SysRT, you have to download systemc2.3.1 and compile it.

## 4. BUILDING SYSRT LIBRARY 
 To build SysRT library, go to the path where the package is downloaded.
  % make clean
  % make

## 5 RUNNING AN EXAMPLE 
Every directory under one of the "examples" directories contains a
different example. Go inside the directory and run the executable
file.

### 5.1 Setup library path
    Before run an example:
    % source <install_path>/setup.sh
  
### 5.2 run examples locally:
   % cd <example>
   % make test


Jun Xiao
