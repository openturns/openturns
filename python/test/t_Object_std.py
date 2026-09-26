import openturns as ot
import openturns.experimental as otexp
import inspect

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

# find all instantiable classes
persistentClasses = {}
for mod in [ot, otexp]:
    for name, obj in inspect.getmembers(mod):
        if inspect.isclass(obj) and issubclass(obj, ot.PersistentObject):
            persistentClasses[obj.__name__] = obj

# copy ctor
failed = []
for cname, class_ in persistentClasses.items():
    print(cname)
    try:
        instance = class_()
        try:
            instance2 = class_(instance)
            print(cname, "OK")
        except Exception as exc:
            failed += [cname]
            print("--", cname, exc)
    except Exception:
        pass
print(f"==== {len(failed)} failures / {len(persistentClasses)} classes ====")
print(f"failed={failed}")
assert len(failed) < 1, f"{len(failed)} serialization failures: {failed}"
