import openturns as ot
import openturns.experimental as otexp
import inspect
import re

ot.TESTPREAMBLE()
ot.Log.Show(ot.Log.NONE)

# find all instantiable classes
persistentClasses = {}
for mod in [ot, otexp]:
    for name, obj in inspect.getmembers(mod):
        if inspect.isclass(obj) and issubclass(obj, ot.PersistentObject):
            persistentClasses[obj.__name__] = obj

# repr() of a proxy that does not define its own __repr__/__str__ is not comparable
defaultRepr = re.compile(r"proxy of <Swig Object of type")


def isSame(instance, instance2):
    # A same-type argument may be routed to a converting constructor instead of the
    # copy constructor, in which case no exception is raised but the copy is wrong,
    # e.g. ot.SklarCopula(copula) used to build a SklarCopula of itself. Some __repr__
    # are not stable across a copy (TimeSeries holds a reading cursor), hence the
    # fallback on __str__ before reporting a difference.
    repr1, repr2 = repr(instance), repr(instance2)
    if defaultRepr.search(repr1) or defaultRepr.search(repr2):
        return True
    if repr1 != repr2:
        return str(instance) == str(instance2)
    return True


# copy ctor
failed = []
notCopied = []
for cname, class_ in persistentClasses.items():
    print(cname)
    try:
        instance = class_()
        try:
            instance2 = class_(instance)
            if isSame(instance, instance2):
                print(cname, "OK")
            else:
                notCopied += [cname]
                print("--", cname, "not copied:", repr(instance), "->", repr(instance2))
        except Exception as exc:
            failed += [cname]
            print("--", cname, exc)
    except Exception:
        pass
print(f"==== {len(failed)} failures / {len(notCopied)} not copied / {len(persistentClasses)} classes ====")
print(f"failed={failed}")
print(f"notCopied={notCopied}")
assert len(failed) < 1, f"{len(failed)} serialization failures: {failed}"
assert len(notCopied) < 1, f"{len(notCopied)} classes not copied by the copy ctor: {notCopied}"
