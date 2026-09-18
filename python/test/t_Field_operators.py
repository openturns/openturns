#! /usr/bin/env python

import copy
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
mesh = ot.RegularGrid(0, 1, 4)
vals1 = ot.Sample([[1, 2, 3], [2, 3, 4], [3, 4, 5], [4, 5, 6]])
vals2 = ot.Sample([[0, 1, 2], [1, 2, 3], [2, 3, 4], [3, 4, 5]])

f1 = ot.Field(mesh, vals1)
f2 = ot.Field(mesh, vals2)

s = vals1
p = ot.Point((1, 2, 3))

# Field operations
fSum = ot.Field(mesh, vals1 + vals2)
fDiff = ot.Field(mesh, vals1 - vals2)

ott.assert_almost_equal(f1 + f2, fSum)
ott.assert_almost_equal(f1 - f2, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += f2
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= f2
ott.assert_almost_equal(fInPlaceSum, f1)

# Field and Sample operations
fSum = ot.Field(mesh, vals1 + vals1)
fDiff = ot.Field(mesh, vals1 - vals1)

ott.assert_almost_equal(f1 + s, fSum)
ott.assert_almost_equal(f1 - s, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += s
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= s
ott.assert_almost_equal(fInPlaceSum, f1)

# Field and Point operations
fSum = ot.Field(mesh, vals1 + p)
fDiff = ot.Field(mesh, vals1 - p)

ott.assert_almost_equal(f1 + p, fSum)
ott.assert_almost_equal(f1 - p, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += p
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= p
ott.assert_almost_equal(fInPlaceSum, f1)

# Field vs Field operations return a field, see issue #1475
fSumFF = f1 + f2
assert isinstance(fSumFF, ot.Field), "sum should be a Field"
ott.assert_almost_equal(fSumFF.getValues(), vals1 + vals2)
ott.assert_almost_equal(fSumFF.getMesh().getVertices(), mesh.getVertices())
fDiffFF = copy.deepcopy(f1)
fDiffFF -= f2
ott.assert_almost_equal(fDiffFF.getValues(), vals1 - vals2)

# incompatible meshes are rejected
otherMesh = ot.RegularGrid(0, 2, 4)
try:
    f1 + ot.Field(otherMesh, vals1)
    assert False, "different meshes should raise"
except Exception:
    pass

# incompatible dimensions are rejected
try:
    f1 + ot.Field(mesh, vals1.getMarginal(0))
    assert False, "different dimensions should raise"
except Exception:
    pass

# centering a process sample, see issue #1475
processSample = ot.ProcessSample(mesh, 3, 3)
for i in range(3):
    processSample[i] = vals1
meanField = processSample.computeMean()
centered = processSample.getField(0) - meanField
assert isinstance(centered, ot.Field), "centering should give a Field"
ott.assert_almost_equal(centered.getValues(), vals1 - meanField.getValues())
