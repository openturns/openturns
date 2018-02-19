#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test number zero : default constructor and string converter")

    # Default constructor
    symmetricMatrix0 = SymmetricMatrix()

    # String converter
    print("symmetricMatrix0 = ", repr(symmetricMatrix0))

    # TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    print(
        "test number one : constructor with size, operator() and string converter")

    # Constructor with size
    symmetricMatrix1 = SymmetricMatrix(2)

    # Check operator() methods
    symmetricMatrix1[0, 0] = 1.
    symmetricMatrix1[1, 0] = 2.
    symmetricMatrix1[0, 1] = 3.
    symmetricMatrix1[1, 1] = 4.

    # String converter
    print("symmetricMatrix1 = ", repr(symmetricMatrix1))

    # TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER
    print("test number two : copy constructor and string converter")

    # Copy constructor
    symmetricMatrix2 = SymmetricMatrix(symmetricMatrix1)

    # String converter
    print("symmetricMatrix2 = ", repr(symmetricMatrix2))

    # TEST NUMBER THREE : GET DIMENSIONS METHODS
    print("test number three : get dimensions methods")

    # Get dimension methods
    print("symmetricMatrix1's nbRows = ", symmetricMatrix1.getNbRows())
    print("symmetricMatrix1's nbColumns = ", symmetricMatrix1.getNbColumns())

    # TEST NUMBER FIVE : ASSIGNMENT METHOD
    print("test number five : assignment method")

    # Assignment method
    # No sense with pyton

    # TEST NUMBER SIX : TRANSPOSITION METHOD
    print("test number six : transposition method")

    # Check transpose method
    symmetricMatrix4 = symmetricMatrix1.transpose()
    print("symmetricMatrix1 transposed = ", repr(symmetricMatrix4))

    # TEST NUMBER SEVEN : ADDITION METHOD
    print("test number seven : addition method")

    # Check addition method : we check the operator and the symmetry of the
    # operator, thus testing the comparison operator
    sum1 = symmetricMatrix1 + symmetricMatrix4
    sum2 = symmetricMatrix4 + symmetricMatrix1
    print("sum1 = ", repr(sum1))
    print("sum2 = ", repr(sum2))
    print("sum1 equals sum2 = ", sum1 == sum2)

    # TEST NUMBER EIGHT : SUBTRACTION METHOD
    print("test number eight : subtraction method")

    # Check subtraction method
    diff = symmetricMatrix1 - symmetricMatrix4
    print("diff = ", repr(diff))

    # TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD
    print("test number nine : matrix multiplication method")

    # Check multiplication method
    prod = symmetricMatrix1 * symmetricMatrix4
    print("prod = ", repr(prod))

    # TEST NUMBER TEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print("test number ten : multiplication with a numerical point method")

    # Create the numerical point
    pt = Point()
    pt.add(1.)
    pt.add(2.)
    print("pt = ", repr(pt))

    # Check the product method
    ptResult = symmetricMatrix1 * pt
    print("ptResult = ", repr(ptResult))

    # TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR
    # METHODS
    print(
        "test number eleven : multiplication and division by a numerical scalar methods")

    # Check the multiplication method
    s = 3.
    scalprod1 = symmetricMatrix1 * s
    # bug PYTHON scalprod2 = s * symmetricMatrix1
    scalprod3 = symmetricMatrix1 * s
    print("scalprod1 = ", repr(scalprod1))
    # print  "scalprod2 = " , scalprod2
    print("scalprod3 = ", repr(scalprod3))
    # print  "scalprod1 equals scalprod2 = " , (scalprod1 == scalprod2)
    print("scalprod1 equals scalprod3 = ", (scalprod1 == scalprod3))
    # print  "scalprod2 equals scalprod3 = " , (scalprod2 == scalprod3)

    # Check the division method
    scaldiv1 = symmetricMatrix1 / s
    scaldiv2 = symmetricMatrix1 / s
    print("scaldiv1 = ", repr(scaldiv1))
    print("scaldiv2 = ", repr(scaldiv2))
    print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

    # TEST NUMBER TWELVE : ISEMPTY METHOD
    print("test number twelve : isEmpty method")

    # Check method isEmpty
    symmetricMatrix5 = SymmetricMatrix()
    symmetricMatrix6 = SymmetricMatrix()
    print("symmetricMatrix0 is empty = ", symmetricMatrix0.isEmpty())
    print("symmetricMatrix1 is empty = ", symmetricMatrix1.isEmpty())
    print("symmetricMatrix5 is empty = ", symmetricMatrix5.isEmpty())

except:
    import sys
    print("t_SymmetricMatrix_std.py", sys.exc_info()[0], sys.exc_info()[1])
