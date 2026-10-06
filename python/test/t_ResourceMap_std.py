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

# Set with a string value dispatches on the stored type, and it must not throw
# once the value has been stored: a Bool key used to fall through the dispatch
# and raise "key is missing" after having set the value.
ot.ResourceMap.AddAsBool("t_ResourceMap_SetOnBool", False)
ot.ResourceMap.Set("t_ResourceMap_SetOnBool", "true")
assert ot.ResourceMap.GetAsBool("t_ResourceMap_SetOnBool")
ot.ResourceMap.Set("t_ResourceMap_SetOnBool", "false")
assert not ot.ResourceMap.GetAsBool("t_ResourceMap_SetOnBool")
ot.ResourceMap.RemoveKey("t_ResourceMap_SetOnBool")

# the same dispatch for the other stored types
ot.ResourceMap.AddAsString("t_ResourceMap_SetOnString", "a")
ot.ResourceMap.Set("t_ResourceMap_SetOnString", "b")
assert ot.ResourceMap.Get("t_ResourceMap_SetOnString") == "b"
ot.ResourceMap.AddAsScalar("t_ResourceMap_SetOnScalar", 1.0)
ot.ResourceMap.Set("t_ResourceMap_SetOnScalar", "2.5")
assert ot.ResourceMap.GetAsScalar("t_ResourceMap_SetOnScalar") == 2.5
ot.ResourceMap.AddAsUnsignedInteger("t_ResourceMap_SetOnUnsignedInteger", 1)
ot.ResourceMap.Set("t_ResourceMap_SetOnUnsignedInteger", "7")
assert ot.ResourceMap.GetAsUnsignedInteger("t_ResourceMap_SetOnUnsignedInteger") == 7
for key in (
    "t_ResourceMap_SetOnString",
    "t_ResourceMap_SetOnScalar",
    "t_ResourceMap_SetOnUnsignedInteger",
):
    ot.ResourceMap.RemoveKey(key)
