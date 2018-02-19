#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import *

TESTPREAMBLE()

try:

    # DEFAULT CONSTRUCTOR AND STRING CONVERTER
    print("test 0 : default constructor and string converter")

    # Default constructor
    matrix0 = ComplexMatrix()

    # String converter
    print("matrix0 = ", repr(matrix0))

    # CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
    print(
        "test number one : constructor with size, operator() and string converter")

    # Constructor with size
    matrix1 = ComplexMatrix(2, 2)

    # Check operator() methods
    matrix1[0, 0] = 1. + 1j
    matrix1[1, 0] = 2. + 4j
    matrix1[0, 1] = 3. - 1j
    matrix1[1, 1] = 4.

    # String converter
    print("matrix1 = ", repr(matrix1))

    # COPY CONSTRUCTOR AND STRING CONVERTER
    print("test 2 : copy constructor and string converter")

    # Copy constructor
    matrix2 = ComplexMatrix(matrix1)

    # String converter
    print("matrix2 = ", repr(matrix2))

    # GET DIMENSIONS METHODS
    print("test 3 : dimension methods")

    # Get dimension methods
    print("matrix1's nbRows = ", matrix1.getNbRows())
    print("matrix1's nbColumns = ", matrix1.getNbColumns())

    # CONSTRUCTOR WITH COLLECTION
    print("test 4 : constructor with collection method")

    # Create the collection of values
    elementsValues = ComplexCollection()
    elementsValues.add(1. - 1j)
    elementsValues.add(2. - 1j)
    elementsValues.add(3. - 1j)
    elementsValues.add(4. + 1j)
    elementsValues.add(5. + 1j)
    elementsValues.add(6. + 1j)

    # Check the content of the collection
    print("elementsValues = ", repr(elementsValues))

    # Check the constructor with collection
    matrix0bis = ComplexMatrix(2, 2, elementsValues)
    print("matrix0bis = ", repr(matrix0bis))

    # TRANSPOSITION METHOD AND CONJUGATE METHOD
    print("test 5 : transposition / conjugate method")

    # Check transpose method
    matrix4 = matrix1.transpose()
    matrix5 = matrix1.conjugate()
    print("matrix1 transposed = ", repr(matrix4))
    print("matrix1 conjugated = ", repr(matrix5))

    # TRANSPOSITION AND CONJUGATE COUPLED METHOD
    print("transposition and conjugate method")

    # Check transpose method
    matrix6 = matrix1.conjugateTranspose()
    print("matrix1 conjugated and transposed = ", repr(matrix6))

    # ADDITION METHOD
    print("test 6 : addition method")

    # Check addition method : we check the operator and the symmetry of the
    # operator, thus testing the comparison operator
    sum1 = matrix1 + matrix4
    sum2 = matrix4 + matrix1
    print("sum1 = ", repr(sum1))
    print("sum2 = ", repr(sum2))
    print("sum1 equals sum2 = ", sum1 == sum2)

    # SUBTRACTION METHOD
    print("test 7 : subtraction method")

    # Check subtraction method
    diff = matrix1 - matrix4
    print("diff = ", repr(diff))

    #  MATRIX MULTIPLICATION METHOD
    print("test 8 : matrix multiplication method")

    # Check multiplication method
    prod = matrix1 * matrix4
    print("prod = ", repr(prod))

    # MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print("test 9 : multiplication with a numerical point method")

    # Create the numerical point
    pt = Point()
    pt.add(1.)
    pt.add(2.)
    print("pt = ", repr(pt))

    # Check the product method
    ptResult = matrix1 * pt
    print("ptResult = ", repr(ptResult))

    #  MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
    print(
        "test 10 : multiplication and division by a numerical scalar methods")

    # Check the multiplication method
    s = 3. + 1j
    scalprod1 = matrix1 * s
    print("scalprod1 = ", repr(scalprod1))

    # Check the division method
    scaldiv1 = matrix1 / s
    print("scaldiv1 = ", repr(scaldiv1))

    #  ISEMPTY METHOD
    print("test 10 : isEmpty method")

    # Check method isEmpty
    matrix7 = ComplexMatrix()
    matrix8 = ComplexMatrix()

    print("matrix1 is empty = ", matrix1.isEmpty())
    print("matrix5 is empty = ", matrix7.isEmpty())
    print("matrix6 is empty = ", matrix8.isEmpty())
    print("matrix0 is empty = ", matrix0.isEmpty())

    # MULTIPLICATION WITH A NUMERICAL POINT METHOD
    print("test 11 : multiplication with a numerical point method")

    # Create the numerical point
    pt_test = Point()
    pt_test.add(1.)
    pt_test.add(2.)
    print("pt_test = ", repr(pt_test))

    A = ComplexMatrix(2, 2)
    A[0, 0] = 0.5
    A[1, 0] = -(sqrt(3.) / 2)
    A[0, 1] = (sqrt(3.) / 2)
    A[1, 1] = 0.5
    B = A.transpose()
    identity = B * A

    # Check the product method
    ptResult2 = identity * pt_test
    print("A = ", repr(A))
    print("B = ", repr(B))
    print("identity = ", repr(identity))
    print("ptResult2 = ", repr(ptResult2))
except:
    import sys
    print("t_ComplexMatrix_std.py", sys.exc_info()[0], sys.exc_info()[1])
