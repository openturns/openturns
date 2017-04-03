#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ref_values = [[1.0, 0.0], [0.0, 0.5]]

mats = [ot.Matrix(ref_values),
        ot.SquareMatrix(ref_values),
        ot.TriangularMatrix(ref_values),
        ot.SymmetricMatrix(ref_values),
        ot.CovarianceMatrix(ref_values),
        ot.CorrelationMatrix(ref_values)]
mats.extend([
    ot.ComplexMatrix(ref_values),
    ot.HermitianMatrix(ref_values),
    ot.TriangularComplexMatrix(ref_values),
    ot.SquareComplexMatrix(ref_values)])

for a in mats:

    # try conversion
    ref = ot.Matrix([[1.0, 0.0], [0.0, 0.5]])
    iname = a.__class__.__name__
    print('a=', a)

    # try scalar mul
    try:
        s = 5.
        ats = a * s
        print('a*s=', ats)
        sta = s * a
        print('s*a=', sta)
    except:
        print('no scalar mul for', iname)

    # try scalar div
    try:
        s = 5.
        ads = a / s
        print('a/s=', ads)
    except:
        print('no scalar div for', iname)

    # try vec mul
    try:
        x = ot.Point(range(6, 6 + a.getNbColumns()))
        ax = a * x
        print('a*x=', ax)
    except:
        print('no vec mul for', iname)

    try:
        a3 = a ** 3
        print('a**3=', a3)
    except:
        print('no pow for', iname)

    for b in mats:
        jname = b.__class__.__name__
        try:
            ab = a * b
            print('a*b=', ab)
        except:
            print('no mul for', iname, jname)

        try:
            apb = a + b
            print('a+b=', apb)
        except:
            print('no add for', iname, jname)

        try:
            amb = a - b
            print('a-b=', amb)
        except:
            print('no sub for', jname)

sm = ot.SymmetricMatrix(2)
sm[0, 0] = 2.0
sm[1, 1] = 3.0
sm[0, 1] = 0.3
w = ot.Point([1.0, 2.0])
sq = ot.SquareMatrix(sm)
print('sm * w=', sm * w)
print('sq * w=', sq * w)

sm = ot.HermitianMatrix(2)
sm[0, 0] = 2.0
sm[1, 1] = 3.0
sm[1, 0] = 0.3j
w = ot.Point([1.0, 2.0])
sq = ot.SquareComplexMatrix(sm)
print('sm * w=', sm * w)
print('sq * w=', sq * w)
