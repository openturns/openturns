#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    #  Constructor with size
    dimension = 2
    squareMatrix1 = SquareMatrix(dimension)

    squareMatrix1[0, 0] = 1.
    squareMatrix1[1, 0] = 2.
    squareMatrix1[0, 1] = 3.
    squareMatrix1[1, 1] = 4.

    # Second matrix to add to the ARMACoefficients
    s = 3.
    squareMatrix2 = squareMatrix1 * s

    # Second matrix to add to the ARMACoefficients
    t = 1.5
    squareMatrix3 = squareMatrix1 / t

    # size : Number of matrix
    size = 3

    # ARMACoefficients with default constructor
    coefficients0 = ARMACoefficients()
    print("Using default constructor ")
    print("coefficients0 = ", coefficients0)

    # ARMACoefficients with size / dimension constructor
    coefficients1 = ARMACoefficients(size, dimension)
    coefficients1[0] = squareMatrix1
    coefficients1[1] = squareMatrix2
    coefficients1[2] = squareMatrix3

    print("Using constructor based on size / dimension ")
    print("coefficients1 = ", repr(coefficients1))

    # constructors with collection of matrix
    myCollection = SquareMatrixCollection()
    myCollection.add(squareMatrix1)
    myCollection.add(squareMatrix2)
    myCollection.add(squareMatrix3)

    print("SquareMatrix collection = ", repr(myCollection))
    coefficients2 = ARMACoefficients(myCollection)

    # print  of the new collection
    print("Using constructor based on a collection ")
    print("coefficients2 = ", repr(coefficients2))

    # 1D case
    point = Point(dimension * dimension, 0.0)
    point[0] = 1.
    point[1] = 2.
    point[2] = 3.
    point[3] = 4.

    # ARMACoefficients Point constructor
    coefficients3 = ARMACoefficients(point)

    # print  of the new collection
    print("Using Point constructor ")
    print("coefficients3 = ", repr(coefficients3))

    # ARMACoefficients with polynomial constructor
    point *= 2.0
    poly = UniVariatePolynomial(point)

    coefficients4 = ARMACoefficients(poly)

    # print  of the new collection
    print("Using polynomial constructor ")
    print("coefficients4 = ", repr(coefficients4))

except:
    import sys
    print("t_ARMACoefficients_std.py", sys.exc_info()[0], sys.exc_info()[1])
