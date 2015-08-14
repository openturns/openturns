#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

t_names = ['Matrix', 'SquareMatrix', 'TriangularMatrix',
           'SymmetricMatrix', 'CovarianceMatrix', 'CorrelationMatrix']
t_names.extend(
    ['ComplexMatrix', 'HermitianMatrix', 'TriangularComplexMatrix', 'SquareComplexMatrix'])

for i, iname in enumerate(t_names):

    # try conversion
    ref = ot.Matrix([[1.0, 0.0], [0.0, 0.5]])
    a = getattr(ot, iname)(ref)
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
        x = ot.NumericalPoint([6, 7])
        ax = a * x
        print('a*x=', ax)
    except:
        print('no vec mul for', iname)

    try:
        a3 = a ** 3
        print('a**3=', a3)
    except:
        print('no pow for', iname)

    for j, jname in enumerate(t_names):
        b = getattr(ot, iname)(ref)
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
