#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
lifting = ot.KarhunenLoeveLifting(result)
print("lifting=", lifting)
# Get the input and output description
print("lifting input description=", lifting.getInputDescription())
print("lifting output description=", lifting.getOutputDescription())
# Get the input and output dimension
print("lifting input dimension=", lifting.getInputDimension())
print("lifting output dimension=", lifting.getOutputDimension())
point = [1.0] * lifting.getInputDimension()
field = lifting(point)
print("point=", point)
print("field=", field)
# Get the number of calls
print("called ", lifting.getCallsNumber(), " times")

# Extra coverage (no output to preserve expout)
default_lifting = ot.KarhunenLoeveLifting()
assert default_lifting == default_lifting
assert lifting == lifting
assert lifting == ot.KarhunenLoeveLifting(result)
assert not lifting == default_lifting
assert len(repr(lifting)) > 0
assert len(str(lifting)) > 0
with ott.assert_raises(Exception):
    lifting.getMarginal(lifting.getOutputDimension())
with ott.assert_raises(Exception):
    lifting.getMarginal(0)
with ott.assert_raises(Exception):
    lifting.getMarginal([0])
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("lifting_test.xml"))
study.add("lifting", lifting)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager("lifting_test.xml"))
study2.load()
loaded = ot.KarhunenLoeveLifting()
study2.fillObject("lifting", loaded)
assert ot.PointToFieldFunction(loaded) == ot.PointToFieldFunction(lifting)

os.remove("lifting_test.xml")
