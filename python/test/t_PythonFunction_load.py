#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

f = ot.Function()

# load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("pyf.xml"))
study.load()
study.fillObject("f", f)

x = [4, 5]
print(f(x))

os.remove("pyf.xml")
