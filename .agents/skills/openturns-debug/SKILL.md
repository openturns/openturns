---
name: openturns-debug
description: 'Debugging guidelines.'
---

# Debugging OpenTURNS

## When to use
- A test crashes without exploitable exception message, like: `563 - pyinstallcheck_Study_std (Subprocess aborted)`
- A C++ test segfaults or a Python script aborts inside the C++/SWIG layer.

## Build flags
- Reproduce with symbols: configure with the `linux-debug` preset (`CMAKE_BUILD_TYPE=RelWithDebInfo`, see `CMakePresets.json`) before running gdb or valgrind.

## Python
When a Python test crashes one can run gdb on the Python interpreter to retrieve the stack:
```sh
PYTHONPATH=$PWD/build/install/lib/python3.*/site-packages gdb -q -batch -ex run -ex bt --args /usr/bin/python3 ./python/test/t_Axial_std.py
```

## C++
C++ test binaries live under `build/lib/test/` after `cmake --build build --target tests`:
```sh
gdb -q -batch -ex run -ex bt --args ./build/lib/test/t_ResourceMap_std
```

## Memory errors
- For out-of-bounds access or uninitialized values, run the same gdb command under valgrind from a CI-equivalent container (see the `openturns-ci-repro` skill).
