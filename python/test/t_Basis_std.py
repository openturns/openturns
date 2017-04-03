#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import *

ot.TESTPREAMBLE()

try:

    basis = ot.Basis()
    print('basis =', basis)
    basisSize = 3

    phis = []
    for j in range(basisSize):
        phis.append(ot.Function(['x'], ['y'], ['x^' + str(j + 1)]))
    basis = ot.Basis(phis)
    print('basis =', basis)

    print(basis.getSize())
    print(basis.getSubBasis([1, 2]))
    print(basis.isFunctional())
    print(basis.isOrthogonal())
    #print(basis[1])
    #print(basis[0:2])

    #basis[1] = ot.Function(['x'], ['z'], ['x^42'])
    #print('basis =', basis)
    #basis[0:2] = basis[1:3]
    #print('basis =', basis)
except:
    import sys
    print("t_Basis_std.py", sys.exc_info()[0], sys.exc_info()[1])
