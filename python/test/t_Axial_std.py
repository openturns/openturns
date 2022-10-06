#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


center = ot.Point([0.5, 1.5, 2.5, 3.5])
center.setName('center')
levels = ot.Point([4, 8, 16])
levels.setName('levels')
experiment = ot.Axial(center, levels)
print("experiment = ", experiment)
sample = experiment.generate()
print("sample = ", repr(sample))
