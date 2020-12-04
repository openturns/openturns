#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import os

ot.TESTPREAMBLE()

rv = ot.RandomVector()

# load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyrv.xml'))
study.load()
study.fillObject('rv', rv)

print(rv)
print(rv.getRealization())
print(rv.getMean())

os.remove('pyrv.xml')
