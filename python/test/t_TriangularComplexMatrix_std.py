#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    # DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test : default constructor and string converter")

    # Default constructor
    triangularMatrix0 = TriangularComplexMatrix()

    # String converter
    print("triangularMatrix0 = ", repr(triangularMatrix0))

    # CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    print("test : constructor with size, operator() and string converter")

    # Constructor with size
    triangularMatrix1 = TriangularComplexMatrix(2)

    # Check operator() methods
    triangularMatrix1[0, 0] = 1.
    triangularMatrix1[1, 0] = 3. + 1j
    triangularMatrix1[1, 1] = 4.

    # String converter
    print("triangularMatrix1 = ", repr(triangularMatrix1))

    # COPY CONSTRUCTOR AND STRING CONVERTER
    print("test : copy constructor and string converter")

    # Copy constructor
    triangularMatrix2 = TriangularComplexMatrix(triangularMatrix1)

    # String converter
    print("triangularMatrix2 = ", repr(triangularMatrix2))

    # GET DIMENSIONS METHODS
    print("test : get dimensions methods")

    # Get dimension methods
    print("triangularMatrix1's nbRows = ", triangularMatrix1.getNbRows())
    print("triangularMatrix1's nbColumns = ", triangularMatrix1.getNbColumns())
    print("isLower? ", triangularMatrix1.isLowerTriangular())

    # TRANSPOSE AND CONJUGATE METHOD
    print("test  : transpose and conjugate methods")

    # Check conjugate method
    print("triangularMatrix1 conjugate = ",
          repr(triangularMatrix1.conjugate()))
    print("is conjugate Lower? ",
          triangularMatrix1.conjugate().isLowerTriangular())

    print("triangularMatrix1 transpose = ",
          repr(triangularMatrix1.transpose()))
    print("is transpose Lower? ",
          triangularMatrix1.transpose().isLowerTriangular())

    triangularMatrix4 = triangularMatrix1.conjugateTranspose()

    print("triangularMatrix1 conjugate and transpose = ",
          repr(triangularMatrix4))
    print("is conjugate Lower? ", triangularMatrix4.isLowerTriangular())

    #  ADDITION METHOD
    print("test : addition method")

    # Check addition method : we check the operator and the symmetry of the
    # operator, thus testing the comparison operator
    sum1 = triangularMatrix1 + triangularMatrix4
    sum2 = triangularMatrix4 + triangularMatrix1
    print("sum1 = ", repr(sum1))
    print("sum2 = ", repr(sum2))
    print("sum1 equals sum2 = ", sum1 == sum2)

    # SUBTRACTION METHOD
    print("test : subtraction method")

    # Check subtraction method
    diff = triangularMatrix1 - triangularMatrix4
    print("diff = ", repr(diff))

    #  MATRIX MULTIPLICATION METHOD
    print("test : matrix multiplication method")

    # Check multiplication method
    prod = triangularMatrix1 * triangularMatrix4
    print("prod = ", repr(prod))

    # MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print("test : multiplication with a numerical point method")

    # Create the numerical point
    pt = Point()
    pt.add(1.)
    pt.add(2.)
    print("pt = ", repr(pt))

    # Check the product method
    ptResult = triangularMatrix1 * pt
    print("ptResult = ", repr(ptResult))

    # MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
    print("test : multiplication and division by a numerical scalar methods")

    # Check the multiplication method
    s = 3. + 2j
    scalprod = triangularMatrix1 * s

    print("scalprod = ", repr(scalprod))

    # Check the division method
    scaldiv1 = triangularMatrix1 / s
    scaldiv2 = triangularMatrix1 / s
    print("scaldiv1 = ", repr(scaldiv1))
    print("scaldiv2 = ", repr(scaldiv2))
    print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

    # ISEMPTY METHOD
    print("test : isEmpty method")

    # Check method isEmpty
    triangularMatrix5 = TriangularComplexMatrix()
    triangularMatrix6 = TriangularComplexMatrix()
    print("triangularMatrix0 is empty = ", triangularMatrix0.isEmpty())
    print("triangularMatrix1 is empty = ", triangularMatrix1.isEmpty())
    print("triangularMatrix5 is empty = ", triangularMatrix5.isEmpty())

except:
    import sys
    print("t_TriangularComplexMatrix_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
