#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # NonCentralStudent related functions
    # dNonCentralStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    deltaMin = 0.2
    deltaMax = 5.0
    n2 = 5
    xMin = 0.1
    xMax = 0.9
    nX = 5
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for i2 in range(n2):
            delta = deltaMin + (deltaMax - deltaMin) * i2 / (n2 - 1)
            for iX in range(nX):
                x = xMin + (xMax - xMin) * iX / (nX - 1)
                print("dNonCentralStudent(", nu, ", ", delta, ",  %.12g" %
                      x, ")=%.6g" % DistFunc.dNonCentralStudent(nu, delta, x))
    # pNonCentralStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    deltaMin = 0.2
    deltaMax = 5.0
    n2 = 5
    xMin = 0.1
    xMax = 0.9
    nX = 5
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for i2 in range(n2):
            delta = deltaMin + (deltaMax - deltaMin) * i2 / (n2 - 1)
            for iX in range(nX):
                x = xMin + (xMax - xMin) * iX / (nX - 1)
                print("pNonCentralStudent(", nu, ", ", delta, ",  %.12g" % x, ")=%.6g" % DistFunc.pNonCentralStudent(
                    nu, delta, x), ", complementary=%.6g" % DistFunc.pNonCentralStudent(nu, delta, x, True))
    # rNonCentralStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    deltaMin = 0.2
    deltaMax = 5.0
    n2 = 5
    nR = 5
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for i2 in range(n2):
            delta = deltaMin + (deltaMax - deltaMin) * i2 / (n2 - 1)
            for iR in range(nR):
                print("rNonCentralStudent(", nu, ", ", delta, ")=%.6g" %
                      DistFunc.rNonCentralStudent(nu, delta))

except:
    import sys
    print("t_DistFunc_noncentralstudent.py",
          sys.exc_info()[0], sys.exc_info()[1])
