#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

d = ot.Distribution()

# load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyd.xml'))
study.load()
study.fillObject('d', d)

print(d.getKurtosis())
print(d.computePDF([0.5]))
print(d.computeCDF([0.5]))

os.remove('pyd.xml')
