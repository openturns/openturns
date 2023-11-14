#! /usr/bin/env python

import openturns as ot
import inspect

ot.TESTPREAMBLE()

# find all instanciable classes
instanciables = []
for name, obj in inspect.getmembers(ot):
    if inspect.isclass(obj):
        cn = obj.__name__
        if "_" in cn:
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
    count_class += 1
    if class_.__doc__ is None:
        print(f"{class_.__name__} class")
        count_class_undoc += 1
    for attr_name in dir(class_):
        if attr_name[0] == "_":
            continue
        class_attr = getattr(class_, attr_name)
        if callable(class_attr):
            count_methods += 1
            if class_attr.__doc__ is None:
                print(f"{class_.__name__}.{attr_name} method")
                count_methods_undoc += 1

print(
    f"-- undocumented classes: {count_class_undoc} ({100.0 * count_class_undoc / count_class:.2f}%) --"
)
print(
    f"-- undocumented methods: {count_methods_undoc} ({100.0 * count_methods_undoc / count_methods:.2f}%) --"
)
if count_class_undoc + count_methods_undoc > 600:
    raise ValueError(
        f"too much undocumented class/methods ({count_class_undoc + count_methods_undoc})"
    )
