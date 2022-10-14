#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

f = ot.PointToFieldFunction()

# load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("pyp2ff.xml"))
study.load()
study.fillObject("f", f)

x = [4, 5]
y = f(x)
print(y[0])
assert y[0] == [5.0], "wrong y value"

os.remove("pyp2ff.xml")
