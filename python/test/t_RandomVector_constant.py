#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# We create a numerical point of dimension 4
point = ot.Point([101.0, 102.0, 103.0, 104.0])

print("point = ", repr(point))

# We create a 'constant' RandomVector from the Point
vect = ot.RandomVector(ot.ConstantRandomVector(point))
print("vect=", vect)

# Check standard methods of class RandomVector
print("vect dimension=", vect.getDimension())
print("vect realization (first )=", repr(vect.getRealization()))
print("vect realization (second)=", repr(vect.getRealization()))
print("vect realization (third )=", repr(vect.getRealization()))
print("vect sample =", repr(vect.getSample(5)))

assert "ConstantRandomVector" in repr(ot.ConstantRandomVector(point))
ott.assert_almost_equal(vect.getMean(), point)
ott.assert_almost_equal(
    ot.Matrix(vect.getCovariance()), ot.Matrix(4, 4)
)
ott.assert_almost_equal(vect.getMarginal(1).getRealization(), [102.0])
ott.assert_almost_equal(
    vect.getMarginal([3, 1]).getRealization(), [104.0, 102.0]
)
with ott.assert_raises(Exception):
    vect.getMarginal(4)
with ott.assert_raises(Exception):
    vect.getMarginal([0, 4])
assert "Dirac" in repr(vect.getDistribution())
ott.assert_almost_equal(vect.getDistribution().getMean(), point)
assert vect.getParameter() == []
assert vect.getParameterDescription() == []
vect.setParameter([])
with ott.assert_raises(Exception):
    vect.setParameter([1.0])

# PointWithDescription constructor keeps the description
namedPoint = ot.PointWithDescription(ot.Point([101.0, 102.0]))
namedPoint.setDescription(["a", "b"])
namedVect = ot.RandomVector(ot.ConstantRandomVector(namedPoint))
assert namedVect.getDescription() == ["a", "b"]

# Default constructor
assert ot.ConstantRandomVector().getDimension() == 0

# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("crv.xml"))
study.add("vect", vect)
study.save()
loaded = ot.RandomVector()
reloader = ot.Study()
reloader.setStorageManager(ot.XMLStorageManager("crv.xml"))
reloader.load()
reloader.fillObject("vect", loaded)
ott.assert_almost_equal(loaded.getRealization(), point)
os.remove("crv.xml")
