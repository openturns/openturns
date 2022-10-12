#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# DEFAULT CONSTRUCTOR AND STRING CONVERTER
print("test : default constructor and string converter")

# Default constructor
hermitianMatrix0 = ot.HermitianMatrix()

# String converter
print("hermitianMatrix0 = ", repr(hermitianMatrix0))

# CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
print("test : constructor with size, operator() and string converter")

# Constructor with size
hermitianMatrix1 = ot.HermitianMatrix(2)

# Check operator() methods
hermitianMatrix1[0, 0] = 1.0
hermitianMatrix1[1, 0] = 3.0 + 1.0j
hermitianMatrix1[1, 1] = 4.0

# String converter
print("hermitianMatrix1 = ", repr(hermitianMatrix1))

# COPY CONSTRUCTOR AND STRING CONVERTER
print("test : copy constructor and string converter")

# Copy constructor
hermitianMatrix2 = ot.HermitianMatrix(hermitianMatrix1)

# String converter
print("hermitianMatrix2 = ", repr(hermitianMatrix2))

# GET DIMENSIONS METHODS
print("test : get dimensions methods")

# Get dimension methods
print("hermitianMatrix1's nbRows = ", hermitianMatrix1.getNbRows())
print("hermitianMatrix1's nbColumns = ", hermitianMatrix1.getNbColumns())

# CONJUGATE METHOD
print("test  : conjugate method")

# Check conjugate method
hermitianMatrix4 = hermitianMatrix1.conjugate()
print("hermitianMatrix1 conjugate = ", repr(hermitianMatrix4))

#  ADDITION METHOD
print("test : addition method")

# Check addition method : we check the operator and the symmetry of the
# operator, thus testing the comparison operator
sum1 = hermitianMatrix1 + hermitianMatrix4
sum2 = hermitianMatrix4 + hermitianMatrix1
print("sum1 = ", repr(sum1))
print("sum2 = ", repr(sum2))
print("sum1 equals sum2 = ", sum1 == sum2)

# SUBTRACTION METHOD
print("test : subtraction method")

# Check subtraction method
diff = hermitianMatrix1 - hermitianMatrix4
print("diff = ", repr(diff))

#  MATRIX MULTIPLICATION METHOD
print("test : matrix multiplication method")

# Check multiplication method
prod = hermitianMatrix1 * hermitianMatrix4
print("prod = ", repr(prod))

# MULTIPLICATION WITH A NUMERICAL POINT METHOD
print("test : multiplication with a numerical point method")

# Create the numerical point
pt = ot.Point()
pt.add(1.0)
pt.add(2.0)
print("pt = ", repr(pt))

# Check the product method
ptResult = hermitianMatrix1 * pt
print("ptResult = ", repr(ptResult))

# MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
print("test : multiplication and division by a numerical scalar methods")

# Check the multiplication method
s = 3.0 + 2j
scalprod = hermitianMatrix1 * s

print("scalprod = ", repr(scalprod))

# Check the division method
scaldiv1 = hermitianMatrix1 / s
scaldiv2 = hermitianMatrix1 / s
print("scaldiv1 = ", repr(scaldiv1))
print("scaldiv2 = ", repr(scaldiv2))
print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

# ISEMPTY METHOD
print("test : isEmpty method")

# Check method isEmpty
hermitianMatrix5 = ot.HermitianMatrix()
hermitianMatrix6 = ot.HermitianMatrix()
print("hermitianMatrix0 is empty = ", hermitianMatrix0.isEmpty())
print("hermitianMatrix1 is empty = ", hermitianMatrix1.isEmpty())
print("hermitianMatrix5 is empty = ", hermitianMatrix5.isEmpty())
