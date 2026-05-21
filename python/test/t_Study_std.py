#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import os
import inspect

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

# find all instantiable classes
persistentClasses = {}
for mod in [ot, otexp]:
    for name, obj in inspect.getmembers(mod):
        if inspect.isclass(obj) and issubclass(obj, ot.PersistentObject):
            persistentClasses[obj.__name__] = obj

# save / load
fileName = "studyStd.xml"
failed = []
for cname, class_ in persistentClasses.items():
    study = ot.Study()
    study.setStorageManager(ot.XMLStorageManager(fileName))
    print(cname)
    try:
        instance = class_()
        study.add(cname, instance)
        study.save()
        study = ot.Study()
        study.setStorageManager(ot.XMLStorageManager(fileName))
        study.load()
        os.remove(fileName)
        instance = class_()
        study.fillObject(cname, instance)
        print(cname, "OK")
    except Exception as exc:
        failed += [cname]
        print("--", cname, exc)
print(f"==== {len(failed)} failures / {len(persistentClasses)} classes ====")
print(f"failed={failed}")
assert len(failed) < 43, f"{len(failed)} serialization failures: {failed}"

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

# uft8 BOM
fileName = "study_bom.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
study.add("x", ot.Point([42.0]))
study.save()
# prepend BOM
with open(fileName, "rb") as f:
    data = f.read()
with open(fileName, "wb") as f:
    f.write(b"\xEF\xBB\xBF")
    f.write(data)
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
study.load()
os.remove(fileName)

# test nested object save/load (issue #2551)
fileName = "study_nested.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fileName))
f = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)
x = ot.Normal(2).getSample(50)
y = f(x)
algo = ot.LinearLeastSquares(x, y)
algo.run()
mm = algo.getResult().getMetaModel()
study.add("mm", mm)
study.add("algo", algo)
study.save()

study = ot.Study(fileName)
study.load()
mm2 = ot.Function()
study.fillObject("mm", mm2)
algo2 = ot.LinearLeastSquares()
study.fillObject("algo", algo2)
os.remove(fileName)
