#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

print("Catalog={")
for key in Catalog.GetKeys():
    print("  %s," % key)
print("}")
