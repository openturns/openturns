#!/usr/bin/env python

import inspect
import openturns as ot
from pathlib import Path
import re

ot.TESTPREAMBLE()

# find all rst files
rst_files = (
    Path(__file__).parents[1].joinpath("doc").joinpath("user_manual").rglob("*.rst")
)
rst_lines = []
for rst_file in rst_files:
    with open(rst_file) as f:
        rst_lines += f.read().splitlines()

# find all instanciable classes
instanciables = []
for name, obj in inspect.getmembers(ot):
    if inspect.isclass(obj):
        cn = obj.__name__
        if (
            cn.startswith("_")
            or cn.endswith("Collection")
            or cn.endswith("Implementation")
        ):
            continue
        try:
            instance = obj()
            instanciables.append(obj)
        except Exception:
            pass

# find missing docstrings
count_class = 0
count_class_undoc = 0
count_methods = 0
count_methods_undoc = 0

for class_ in instanciables:
    cn = class_.__name__
    count_class += 1
    found = False
    for line in rst_lines:
        if re.search(rf"{cn}", line) is not None:
            found = True
            break
    if not found:
        count_class_undoc += 1
        print(f"{cn} class")

# find all static functions
for name, mod in inspect.getmembers(ot):
    if inspect.ismodule(mod):
        modn = mod.__name__
        if not modn.startswith("openturns") or modn == "openturns":
            continue
        if "_" in modn:
            continue

        for attr_name in dir(mod):
            if "_" in attr_name:
                continue
            obj = getattr(mod, attr_name)

            if not inspect.isclass(obj) and callable(obj):
                symboln = f"{modn.split('.')[1]}.{attr_name}"
                count_methods += 1
                found = False
                for line in rst_lines:
                    if re.search(rf"\b{symboln}\b", line) is not None:
                        found = True
                        break
                if not found:
                    count_methods_undoc += 1
                    print(f"{symboln} method")

print(
    f"-- undocumented classes: {count_class_undoc} ({100.0 * count_class_undoc / count_class:.2f}%) --"
)
print(
    f"-- undocumented methods: {count_methods_undoc} ({100.0 * count_methods_undoc / count_methods:.2f}%) --"
)
if count_class_undoc + count_methods_undoc > 50:
    raise ValueError("too much undocumented class/methods")
