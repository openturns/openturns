#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

inputDimension = 3

myFunction = ot.TranslationFunction([2.0, 3.0, -4.0])
myFunction.setName("translationFunction")
inPoint = ot.Point(inputDimension)
inPoint[0] = 7.0
inPoint[1] = 8.0
inPoint[2] = 9.0
outPoint = myFunction(inPoint)
print("myFunction=", repr(myFunction))
print(myFunction.getName(), "( ", repr(inPoint), " ) = ", repr(outPoint))
print(
    myFunction.getName(),
    ".gradient( ",
    repr(inPoint),
    " ) = ",
    repr(myFunction.gradient(inPoint)),
)
print(
    myFunction.getName(),
    ".hessian( ",
    repr(inPoint),
    " ) = ",
    repr(myFunction.hessian(inPoint)),
)

# TranslationEvaluation extra coverage (no prints)
ev = ot.TranslationEvaluation([2.0, 3.0, -4.0])
_ = repr(ev)
_ = str(ev)
assert ev.getInputDimension() == 3
assert ev.getOutputDimension() == 3
assert ev.isLinear()
assert ev.isLinearlyDependent(0)
ott.assert_almost_equal(ev(inPoint), outPoint, 1e-14, 1e-14)
ott.assert_almost_equal(ev(ot.Sample([inPoint, inPoint])), [outPoint, outPoint], 1e-14, 1e-14)
ott.assert_almost_equal(ev.getConstant(), [2.0, 3.0, -4.0], 1e-14, 1e-14)
ev.setConstant([1.0, 1.0, 1.0])
ott.assert_almost_equal(ev(inPoint), inPoint + [1.0, 1.0, 1.0], 1e-14, 1e-14)
with ott.assert_raises(Exception):
    ev.setConstant([1.0])
with ott.assert_raises(Exception):
    ev([1.0])
with ott.assert_raises(Exception):
    ev(ot.Sample([[1.0]]))
with ott.assert_raises(Exception):
    ev.isLinearlyDependent(5)
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("translation_eval.xml"))
study.add("ev", ot.Function(ev))
study.save()
