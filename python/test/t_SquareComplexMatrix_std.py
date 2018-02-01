#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from math import *


# DEFAULT CONSTRUCTOR AND STRING CONVERTER
print('test : default constructor and string converter')

# Default constructor
squareComplexMatrix0 = ot.SquareComplexMatrix()

# String converter
print('squareComplexMatrix0 = ', repr(squareComplexMatrix0))

# CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
print('test : constructor with size, operator() and string converter')

# Constructor with size
squareComplexMatrix1 = ot.SquareComplexMatrix(2)

# Check operator() methods
squareComplexMatrix1[0, 0] = 1. + 1j
squareComplexMatrix1[0, 1] = 3. + 1j
squareComplexMatrix1[1, 0] = 1.0j
squareComplexMatrix1[1, 1] = 5. + 1.j

# String converter
print('squareComplexMatrix1 = ', repr(squareComplexMatrix1))

# COPY CONSTRUCTOR AND STRING CONVERTER
print('test : copy constructor and string converter')

# Copy constructor
squareComplexMatrix2 = ot.SquareComplexMatrix(squareComplexMatrix1)

# String converter
print('squareComplexMatrix2 = ', repr(squareComplexMatrix2))

# GET DIMENSIONS METHODS
print('test : get dimensions methods')

# Get dimension methods
print('squareComplexMatrix1\'s nbRows = ', squareComplexMatrix1.getNbRows())
print('squareComplexMatrix1\'s nbColumns = ',
      squareComplexMatrix1.getNbColumns())

# CONJUGATE METHOD
print('test  : conjugate method')

# Check conjugate method
squareComplexMatrix4 = squareComplexMatrix1.conjugate()
print('squareComplexMatrix1 conjugate = ', repr(squareComplexMatrix4))

#  ADDITION METHOD
print('test : addition method')

# Check addition method : we check the operator and the symmetry of the
# operator, thus testing the comparison operator
sum1 = squareComplexMatrix1 + squareComplexMatrix4
sum2 = squareComplexMatrix4 + squareComplexMatrix1
print('sum1 = ', repr(sum1))
print('sum2 = ', repr(sum2))
print('sum1 equals sum2 = ', sum1 == sum2)

# SUBTRACTION METHOD
print('test : subtraction method')

# Check subtraction method
diff = squareComplexMatrix1 - squareComplexMatrix4
print('diff = ', repr(diff))

#  MATRIX MULTIPLICATION METHOD
print('test : matrix multiplication method')

# Check multiplication method
prod = squareComplexMatrix1 * squareComplexMatrix4
print('prod = ', repr(prod))

# MULTIPLICATION WITH A NUMERICAL POINT METHOD
print('test : multiplication with a numerical point method')

# Create the numerical point
pt = ot.ComplexCollection(2)
pt[0] = 1. + 1j
pt[1] = 1j
print('pt = ', repr(pt))

# Check the product method
ptResult = squareComplexMatrix1 * pt
print('ptResult = ', repr(ptResult))

# MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
print('test : multiplication and division by a numerical scalar methods')

# Check the multiplication method
s = 3. + 2j
scalprod = squareComplexMatrix1 * s

print('scalprod = ', repr(scalprod))

# Check the division method
scaldiv1 = squareComplexMatrix1 / s
scaldiv2 = squareComplexMatrix1 / s
print('scaldiv1 = ', repr(scaldiv1))
print('scaldiv2 = ', repr(scaldiv2))
print('scaldiv1 equals scaldiv2 = ', (scaldiv1 == scaldiv2))

# ISEMPTY METHOD
print('test : isEmpty method')

# Check method isEmpty
squareComplexMatrix5 = ot.SquareComplexMatrix()
squareComplexMatrix6 = ot.SquareComplexMatrix()
print('squareComplexMatrix0 is empty = ', squareComplexMatrix0.isEmpty())
print('squareComplexMatrix1 is empty = ', squareComplexMatrix1.isEmpty())
print('squareComplexMatrix5 is empty = ', squareComplexMatrix5.isEmpty())
