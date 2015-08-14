#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # pStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    xMin = 0.1
    xMax = 0.9
    nX = 10
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for iX in range(nX):
            x = xMin + (xMax - xMin) * iX / (nX - 1)
            print("pStudent(", nu, ",  %.12g" % x, ")=%.6g" % DistFunc.pStudent(
                nu, x), ", complementary=%.6g" % DistFunc.pStudent(nu, x, True))
    # qStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    qMin = 0.1
    qMax = 0.9
    nQ = 10
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for iQ in range(nQ):
            q = qMin + (qMax - qMin) * iQ / (nQ - 1)
            print("qStudent(", nu, ",  %.12g" % q, ")=%.6g" % DistFunc.qStudent(
                nu, q), ", complementary=%.6g" % DistFunc.qStudent(nu, q, True))
    # rStudent
    nuMin = 0.2
    nuMax = 5.0
    n1 = 5
    nR = 10
    for i1 in range(n1):
        nu = nuMin + (nuMax - nuMin) * i1 / (n1 - 1)
        for iR in range(nR):
            print("rStudent(", nu, ")=%.6g" % DistFunc.rStudent(nu))

except:
    import sys
    print("t_DistFunc_student.py", sys.exc_info()[0], sys.exc_info()[1])
