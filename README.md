# Custom Memory Management in C

This repository contains a custom implementation of memory management functions in C. It provides our own versions of `malloc`, `free`, `calloc`, and `realloc` functions, designed to manage memory allocation, deallocation, and resizing in a controlled manner.

## Features

- **Custom `malloc` Implementation (`HmmAlloc`)**: Allocates memory blocks.
- **Custom `free` Implementation (`HmmFree`)**: Frees allocated memory blocks.
- **Custom `calloc` Implementation (`HmmCalloc`)**: Allocates memory for an array of elements, initializing them to zero.
- **Custom `realloc` Implementation (`HmmRealloc`)**: Resizes an existing memory block.
- **Debugging Support**: Includes a function to print the current state of the heap to help with debugging memory issues.

**Custom Structure Implementation

  HMM.c: C source file for HMM functions.

  test.c: Test program that utilizes the HMM library.

**Custom Build Directories Implementation

  ./build/: Main directory for build files.

  ./build/shared/: Directory for shared library build files.

  ./build/static/: Directory for static library build files.

**Custom General Setup Implementation

First, create the build directory if it doesn't already exist:

mkdir -p ./build

Shared Library

**Custom Compile the HMM object file for shared library: Implementation

gcc -c -fPIC -o ./build/shared/IHMM.o HMM.c

Create the shared library:

gcc --shared -o ./build/shared/libHMM.so ./build/shared/IHMM.o

**Custom Compile the test program using the shared library: Implementation

gcc ./build/test.o ./build/shared/libHMM.so -o ./build/shared/test.out

**Custom Static Library Implementation

Compile the HMM object file for static library:

gcc -c HMM.c -o ./build/static/HMM.o

**Custom Create the static library: Implementation

ar -r ./build/static/lib.a ./build/static/HMM.o

**Custom Compile the test program using the static library:

gcc ./build/test.o ./build/static/lib.a -o ./build/static/test.out

**Custom Compile the Test Program Implementation

To compile the test source file for use with either library configuration:

gcc -c test.c -o ./build/test.o

**Custom Clean Build Files Implementation

  To clean up all generated files and directories:
  
  rm -rf ./build

**Custom Running the Test Program Implementation

  For shared library:
  
  export LD_PRELOAD="./build/shared/libHMM.so"
  
  ./build/shared/test.out

  For static library:
  
  ./build/static/test.out

**Custom Unset the LD_PRELOAD if it was set: Implementation

unset LD_PRELOAD
