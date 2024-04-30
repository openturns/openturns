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
