#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


print("Catalog={")
for key in ot.Catalog.GetKeys():
    print("  %s," % key)
print("}")
