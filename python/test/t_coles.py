#! /usr/bin/env python

import openturns as ot
from openturns.usecases import coles

ot.TESTPREAMBLE()

data = coles.Coles().portpirie
print(data.getSize(), data.getDimension())
assert data.getSize() == 65
assert data.getDimension() == 2

data = coles.Coles().fremantle
print(data.getSize(), data.getDimension())
assert data.getSize() == 86
assert data.getDimension() == 3

data = coles.Coles().venice
print(data.getSize(), data.getDimension())
assert data.getSize() == 50
assert data.getDimension() == 11
