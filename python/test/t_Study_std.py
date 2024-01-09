#! /usr/bin/env python

import openturns as ot
import os
import inspect

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

# find all instanciable classes
instanciables = []
for name, obj in inspect.getmembers(ot):
    if inspect.isclass(obj):
        cn = obj.__name__
        if "_" in cn or "Interface" in cn or "Pointer" in cn:
            continue
        try:
            instance = obj()
            print(obj.__name__, "OK")
            instanciables.append(obj)
        except Exception:
            print(obj.__name__, "FAIL")

# save / load
fileName = "studyStd.xml"
for class_ in instanciables:
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))

    print(class_.__name__)
    try:
        instance = class_()
        study.add(class_.__name__, instance)
        study.save()
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(fileName))
        study.load()
        os.remove(fileName)
        instance = class_()
        study.fillObject(class_.__name__, instance)
        print(class_.__name__, "OK")
    except Exception as exc:
        print("--", class_.__name__, exc)

# non-ascii filename
fileName = "utf_é.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
study.add("x", ot.Point([42.0]))
study.save()
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
study.load()
x = ot.Point()
study.fillObject("x", x)
assert len(x) == 1 and x[0] == 42.0, "wrong x"
print(x)
os.remove(fileName)
