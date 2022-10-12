#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

print("ResourceMap={")
for key in ot.ResourceMap.GetKeys():
    print("  %s => %s," % (key, ot.ResourceMap.Get(key)))
print("}")
print(
    "Extract from ResourceMap: Path-TemporaryDirectory -> ",
    ot.ResourceMap.Get("Path-TemporaryDirectory"),
)
