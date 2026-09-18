---
name: openturns-debug
description: 'Debugging guidelines.'
---

# Debugging OpenTURNS

## When to use
- A test crashes without exploitable exception message.

## Python
When a Python sample crashes one can run gdb on the Python interpreter to retrieve the stack:
```sh
PYTHONPATH=$PWD/build/install/lib/python3.14/site-packages gdb -q batch -ex run -ex bt --args /usr/bin/python3 ./python/test/t_Axial_std.py
```
