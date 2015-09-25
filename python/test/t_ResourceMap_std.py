#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

print("ResourceMap={")
for key in ResourceMap.GetKeys():
    print("  %s => %s," % (key, ResourceMap.Get(key)))
print("}")
print("Extract from ResourceMap : R-executable-command -> ",
      ResourceMap.Get("R-executable-command"))
