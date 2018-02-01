#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()

try:
    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER */
    print("test number zero : default constructor and string converter")

    # Default constructor */
    squareMatrix0 = SquareMatrix()

    # String converter */
    print("squareMatrix0 = ", repr(squareMatrix0))

    # TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    # */
    print(
        "test number one : constructor with size, operator() and string converter")

    # Constructor with size */
    squareMatrix1 = SquareMatrix(2)

    # Check operator() methods */
    squareMatrix1[0, 0] = 1.
    squareMatrix1[1, 0] = 2.
    squareMatrix1[0, 1] = 3.
    squareMatrix1[1, 1] = 4.

    # String converter */
    print("squareMatrix1 = ", repr(squareMatrix1))

    # TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER */
    print("test number two : copy constructor and string converter")

    # Copy constructor */
    squareMatrix2 = SquareMatrix(squareMatrix1)

    # String converter */
    print("squareMatrix2 = ", repr(squareMatrix2))

    # TEST NUMBER THREE : GET DIMENSIONS METHODS */
    print("test number three : get dimensions methods")

    # Get dimension methods */
    print("squareMatrix1's nbRows = ", squareMatrix1.getNbRows())
    print("squareMatrix1's nbColumns = ", squareMatrix1.getNbColumns())

    # TEST NUMBER FIVE : ASSIGNMENT METHOD */
    print("test number five : assignment method")

    # Assignment method */
    # non sense with python

    # TEST NUMBER SIX : TRANSPOSITION METHOD */
    print("test number six : transposition method")

    # Check transpose method */
    squareMatrix4 = squareMatrix1.transpose()
    print("squareMatrix1 transposed = ", repr(squareMatrix4))
    print("squareMatrix1 transposed = ", repr(squareMatrix1.transpose()))

    # TEST NUMBER SEVEN : ADDITION METHOD */
    print("test number seven : addition method")

    # Check addition method : we check the operator and the symmetry of the
    # operator, thus testing the comparison operator */
    sum1 = squareMatrix1 + squareMatrix4
    sum2 = squareMatrix4 + squareMatrix1
    print("sum1 = ", repr(sum1))
    print("sum2 = ", repr(sum2))
    print("sum1 equals sum2 = ", (sum1 == sum2))

    # TEST NUMBER EIGHT : SUBTRACTION METHOD */
    print("test number eight : subtraction method")

    # Check subtraction method */
    diff = squareMatrix1 - squareMatrix4
    print("diff = ", repr(diff))

    # TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD */
    print("test number nine : matrix multiplication method")

    # Check multiplication method */
    prod = squareMatrix1 * squareMatrix4
    print("prod = ", repr(prod))

    # TEST NUMBER TEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD */
    print("test number ten : multiplication with a numerical point method")

    # Create the numerical point */
    pt = Point()
    pt.add(1.)
    pt.add(2.)
    print("pt = ", repr(pt))

    # Check the product method */
    ptResult = squareMatrix1 * pt
    print("ptResult = ", repr(ptResult))

    # TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR
    # METHODS */
    print(
        "test number eleven : multiplication and division by a numerical scalar methods")

    # Check the multiplication method */
    s = 3.
    scalprod1 = squareMatrix1 * s
    # bug PYTHON scalprod2 = s * matrix1
    scalprod2 = squareMatrix1 * s
    print("scalprod1 = ", repr(scalprod1))
    print("scalprod2 = ", repr(scalprod2))
    print("scalprod1 equals scalprod2 = ", (scalprod1 == scalprod2))

    # Check the division method */
    scaldiv1 = squareMatrix1 / s
    scaldiv2 = squareMatrix1 / s
    print("scaldiv1 = ", repr(scaldiv1))
    print("scaldiv2 = ", repr(scaldiv2))
    print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

    # TEST NUMBER TWELVE : ISEMPTY METHOD */
    print("test number twelve : isEmpty method")

    # Check method isEmpty */
    squareMatrix5 = SquareMatrix()
    print("squareMatrix0 is empty = ", squareMatrix0.isEmpty())
    print("squareMatrix1 is empty = ", squareMatrix1.isEmpty())
    print("squareMatrix5 is empty = ", squareMatrix5.isEmpty())

except:
    import sys
    print("t_SquareMatrix_std.py", sys.exc_info()[0], sys.exc_info()[1])
