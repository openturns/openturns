#! /usr/bin/env python

import copy
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()
mesh = ot.RegularGrid(0, 1, 4)
vals1 = [
    ot.Sample([[1, 2, 3], [2, 3, 4], [3, 4, 5], [4, 5, 6]]),
    ot.Sample([[5, 6, 7], [6, 7, 8], [7, 8, 9], [8, 9, 10]]),
]
vals2 = [
    ot.Sample([[0, 1, 2], [1, 2, 3], [2, 3, 4], [3, 4, 5]]),
    ot.Sample([[4, 5, 6], [5, 6, 7], [6, 7, 8], [7, 8, 9]]),
]

ps1 = ot.ProcessSample(mesh, vals1)
ps2 = ot.ProcessSample(mesh, vals2)

f1 = ot.Field(mesh, vals1[0])
f2 = ot.Field(mesh, vals2[0])

s = vals1[0]
p = ot.Point((1, 2, 3))

# ProcessSample operations
sumVals = [vals1[0]+vals2[0], vals1[1]+vals2[1]]
diffVals = [vals1[0]-vals2[0], vals1[1]-vals2[1]]

psSum = ot.ProcessSample(mesh, sumVals)
psDiff = ot.ProcessSample(mesh, diffVals)

ott.assert_almost_equal(ps1 + ps2, psSum)
ott.assert_almost_equal(ps1 - ps2, psDiff)
psInPlaceSum = copy.deepcopy(ps1)
psInPlaceSum += ps2
ott.assert_almost_equal(psInPlaceSum, psSum)
psInPlaceSum -= ps2
ott.assert_almost_equal(psInPlaceSum, ps1)

# ProcessSample  and Field operations
sumVals = [vals1[0]+vals1[0], vals1[1]+vals1[0]]
diffVals = [vals1[0]-vals1[0], vals1[1]-vals1[0]]

psSum = ot.ProcessSample(mesh, sumVals)
psDiff = ot.ProcessSample(mesh, diffVals)

ott.assert_almost_equal(ps1 + f1, psSum)
ott.assert_almost_equal(ps1 - f1, psDiff)
psInPlaceSum = copy.deepcopy(ps1)
psInPlaceSum += f1
ott.assert_almost_equal(psInPlaceSum, psSum)
psInPlaceSum -= f1
ott.assert_almost_equal(psInPlaceSum, ps1)

# ProcessSample and Sample operations
ott.assert_almost_equal(ps1 + s, psSum)
ott.assert_almost_equal(ps1 - s, psDiff)
psInPlaceSum = copy.deepcopy(ps1)
psInPlaceSum += s
ott.assert_almost_equal(psInPlaceSum, psSum)
psInPlaceSum -= s
ott.assert_almost_equal(psInPlaceSum, ps1)

# ProcessSample and point operations
sumVals = [vals1[0]+p, vals1[1]+p]
diffVals = [vals1[0]-p, vals1[1]-p]

psSum = ot.ProcessSample(mesh, sumVals)
psDiff = ot.ProcessSample(mesh, diffVals)

ott.assert_almost_equal(ps1 + p, psSum)
ott.assert_almost_equal(ps1 - p, psDiff)
psInPlaceSum = copy.deepcopy(ps1)
psInPlaceSum += p
ott.assert_almost_equal(psInPlaceSum, psSum)
psInPlaceSum -= p
ott.assert_almost_equal(psInPlaceSum, ps1)

# Field operations
fSum = ot.Field(mesh, vals1[0]+vals2[0])
fDiff = ot.Field(mesh,vals1[0]-vals2[0])

ott.assert_almost_equal(f1 + f2, fSum)
ott.assert_almost_equal(f1 - f2, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += f2
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= f2
ott.assert_almost_equal(fInPlaceSum, f1)

# Field and Sample operations
fSum = ot.Field(mesh, vals1[0]+vals1[0])
fDiff = ot.Field(mesh,vals1[0]-vals1[0])

ott.assert_almost_equal(f1 + s, fSum)
ott.assert_almost_equal(f1 - s, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += s
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= s
ott.assert_almost_equal(fInPlaceSum, f1)

# Field and Point operations
fSum = ot.Field(mesh, vals1[0]+p)
fDiff = ot.Field(mesh,vals1[0]-p)

ott.assert_almost_equal(f1 + p, fSum)
ott.assert_almost_equal(f1 - p, fDiff)
fInPlaceSum = copy.deepcopy(f1)
fInPlaceSum += p
ott.assert_almost_equal(fInPlaceSum, fSum)
fInPlaceSum -= p
ott.assert_almost_equal(fInPlaceSum, f1)
