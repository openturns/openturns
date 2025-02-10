#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

print("ResourceMap={")
for key in ot.ResourceMap.GetKeys():
    print("  %s => %s," % (key, ot.ResourceMap.Get(key)))
print("}")
print(
    "Extract from ResourceMap: Cache-MaxSize -> ",
    ot.ResourceMap.Get("Cache-MaxSize"),
)

# check string enum api
ot.ResourceMap.AddAsString("bar", "v")
ot.ResourceMap.AddAsString("foo", "a", ["a", "b"])
assert ot.ResourceMap.HasStringEnum("foo")
assert ot.ResourceMap.GetStringEnum("foo") == ("a", "b")
ot.ResourceMap.SetAsString("foo", "b")
ok = False
try:
    ot.ResourceMap.SetAsString("foo", "z")
except Exception:
    ok = True
assert ok
ot.ResourceMap.RemoveKey("foo")
ot.ResourceMap.AddAsString("foo", "a")
assert not ot.ResourceMap.HasStringEnum("foo")
