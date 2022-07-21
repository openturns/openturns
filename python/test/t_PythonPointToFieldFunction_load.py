#! /usr/bin/env python

import openturns as ot
import os

ot.TESTPREAMBLE()

f = ot.PointToFieldFunction()

# load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager('pyp2ff.xml'))
study.load()
study.fillObject('f', f)

x = [4, 5]
print(f(x))

os.remove('pyp2ff.xml')
