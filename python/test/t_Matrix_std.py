#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:
    # TEST NUMBER ZERO : DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test number zero : default constructor and string converter")

    # Default constructor
    matrix0 = Matrix()

    # String converter
    print("matrix0 = ", repr(matrix0))

    # TEST NUMBER ONE : CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    print(
        "test number one : constructor with size, operator() and string converter")

    # Constructor with size
    matrix1 = Matrix(2, 2)

    # Check operator() methods
    matrix1[0, 0] = 1.
    matrix1[1, 0] = 2.
    matrix1[0, 1] = 3.
    matrix1[1, 1] = 4.

    # String converter
    print("matrix1 = ", repr(matrix1))

    # TEST NUMBER TWO : COPY CONSTRUCTOR AND STRING CONVERTER
    print("test number two : copy constructor and string converter")

    # Copy constructor
    matrix2 = Matrix(matrix1)

    # String converter
    print("matrix2 = ", repr(matrix2))

    # TEST NUMBER THREE : GET DIMENSIONS METHODS
    print("test number three : get dimensions methods")

    # Get dimension methods
    print("matrix1's nbRows = ", matrix1.getNbRows())
    print("matrix1's nbColumns = ", matrix1.getNbColumns())

    # TEST NUMBER FOUR : CONSTRUCTOR WITH COLLECTION
    print("test number four : constructor with collection method")

    # Create the collection of values
    elementsValues = ScalarCollection()
    elementsValues.add(1.)
    elementsValues.add(2.)
    elementsValues.add(3.)
    elementsValues.add(4.)
    elementsValues.add(5.)
    elementsValues.add(6.)

    # Check the content of the collection
    print("elementsValues = ", repr(elementsValues))

    # Check the constructor with collection
    matrix0bis = Matrix(2, 2, elementsValues)
    print("matrix0bis = ", repr(matrix0bis))

    # TEST NUMBER FIVE : ASSIGNMENT METHOD
    print("test number five : assignment method")

    # Assignment method
    # No sense with pyton

    # TEST NUMBER SIX : TRANSPOSITION METHOD
    print("test number six : transposition method")

    # Check transpose method
    matrix4 = matrix1.transpose()
    print("matrix1 transposed = ", repr(matrix4))

    # TEST NUMBER SEVEN : ADDITION METHOD
    print("test number seven : addition method")

    # Check addition method : we check the operator and the symmetry of the
    # operator, thus testing the comparison operator
    sum1 = matrix1 + matrix4
    sum2 = matrix4 + matrix1
    print("sum1 = ", repr(sum1))
    print("sum2 = ", repr(sum2))
    print("sum1 equals sum2 = ", sum1 == sum2)

    # TEST NUMBER EIGHT : SUBTRACTION METHOD
    print("test number eight : subtraction method")

    # Check subtraction method
    diff = matrix1 - matrix4
    print("diff = ", repr(diff))

    # TEST NUMBER NINE : MATRIX MULTIPLICATION METHOD
    print("test number nine : matrix multiplication method")

    # Check multiplication method
    prod = matrix1 * matrix4
    print("prod = ", repr(prod))

    # TEST NUMBER TEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print("test number ten : multiplication with a numerical point method")

    # Create the numerical point
    pt = Point()
    pt.add(1.)
    pt.add(2.)
    print("pt = ", repr(pt))

    # Check the product method
    ptResult = matrix1 * pt
    print("ptResult = ", repr(ptResult))

    # TEST NUMBER ELEVEN : MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR
    # METHODS
    print(
        "test number eleven : multiplication and division by a numerical scalar methods")

    # Check the multiplication method
    s = 3.
    scalprod1 = matrix1 * s
    # bug PYTHON scalprod2 = s * matrix1
    scalprod3 = matrix1 * s
    print("scalprod1 = ", repr(scalprod1))
    # print  "scalprod2 = " , scalprod2
    print("scalprod3 = ", repr(scalprod3))
    # print  "scalprod1 equals scalprod2 = " , (scalprod1 == scalprod2)
    print("scalprod1 equals scalprod3 = ", (scalprod1 == scalprod3))
    # print  "scalprod2 equals scalprod3 = " , (scalprod2 == scalprod3)

    # Check the division method
    scaldiv1 = matrix1 / s
    scaldiv2 = matrix1 / s
    print("scaldiv1 = ", repr(scaldiv1))
    print("scaldiv2 = ", repr(scaldiv2))
    print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

    # TEST NUMBER TWELVE : ISEMPTY METHOD
    print("test number twelve : isEmpty method")

    # Check method isEmpty
    matrix5 = Matrix()
    matrix6 = Matrix()
    # matrix6.setDimensions(0,3)
    print("matrix1 is empty = ", matrix1.isEmpty())
    print("matrix5 is empty = ", matrix5.isEmpty())
    print("matrix6 is empty = ", matrix6.isEmpty())
    print("matrix0 is empty = ", matrix0.isEmpty())

    # TEST NUMBER FOURTEEN : MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print(
        "test number fourteen : multiplication with a numerical point method")

    # Create the numerical point
    pt_test = Point()
    pt_test.add(1.)
    pt_test.add(2.)
    print("pt_test = ", repr(pt_test))

    A = Matrix(2, 2)
    A[0, 0] = 0.5
    A[1, 0] = -(sqrt(3.) / 2)
    A[0, 1] = (sqrt(3.) / 2)
    A[1, 1] = 0.5
    B = A.transpose()
    id = B * A

    # Check the product method
    ptResult2 = id * pt_test
    print("A = ", repr(A))
    print("B = ", repr(B))
    print("id = ", repr(id))
    print("ptResult2 = ", repr(ptResult2))

    # TEST NUMBER FIFTEEN : MULTIPLICATION WITH A SAMPLE
    print(
        "test number fifteen : multiplication with a sample")
    s = Sample([[1.0, 3.0, -1.0, -3.0], [-2.0, -5.0, 3.0, 1.0]])
    matrix32 = Matrix(3, 2, [1.0 + i for i in range(6)])
    print("matrix32 = ", repr(matrix32))
    print("s = ", repr(s))
    sampleResult1 = matrix32 * s
    print("matrix32*s = ", repr(sampleResult1))

    # unary minus
    A = Matrix([[1, 2], [3, 4]])
    print(-A)
except:
    import sys
    print("t_Matrix_std.py", sys.exc_info()[0], sys.exc_info()[1])
