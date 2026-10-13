#! /usr/bin/env python

import openturns as ot


# DEFAULT CONSTRUCTOR AND STRING CONVERTER
print("test : default constructor and string converter")

# Default constructor
squareComplexMatrix0 = ot.SquareComplexMatrix()

# String converter
print("squareComplexMatrix0 = ", repr(squareComplexMatrix0))

# CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
print("test : constructor with size, operator() and string converter")

# Constructor with size
squareComplexMatrix1 = ot.SquareComplexMatrix(2)

# Check operator() methods
squareComplexMatrix1[0, 0] = 1.0 + 1j
squareComplexMatrix1[0, 1] = 3.0 + 1j
squareComplexMatrix1[1, 0] = 1.0j
squareComplexMatrix1[1, 1] = 5.0 + 1.0j

# String converter
print("squareComplexMatrix1 = ", repr(squareComplexMatrix1))

# COPY CONSTRUCTOR AND STRING CONVERTER
print("test : copy constructor and string converter")

# Copy constructor
squareComplexMatrix2 = ot.SquareComplexMatrix(squareComplexMatrix1)

# String converter
print("squareComplexMatrix2 = ", repr(squareComplexMatrix2))

# GET DIMENSIONS METHODS
print("test : get dimensions methods")

# Get dimension methods
print("squareComplexMatrix1's nbRows = ", squareComplexMatrix1.getNbRows())
print("squareComplexMatrix1's nbColumns = ", squareComplexMatrix1.getNbColumns())

# CONJUGATE METHOD
print("test  : conjugate method")

# Check conjugate method
squareComplexMatrix4 = squareComplexMatrix1.conjugate()
print("squareComplexMatrix1 conjugate = ", repr(squareComplexMatrix4))

#  ADDITION METHOD
print("test : addition method")

# Check addition method : we check the operator and the symmetry of the
# operator, thus testing the comparison operator
sum1 = squareComplexMatrix1 + squareComplexMatrix4
sum2 = squareComplexMatrix4 + squareComplexMatrix1
print("sum1 = ", repr(sum1))
print("sum2 = ", repr(sum2))
print("sum1 equals sum2 = ", sum1 == sum2)

# SUBTRACTION METHOD
print("test : subtraction method")

# Check subtraction method
diff = squareComplexMatrix1 - squareComplexMatrix4
print("diff = ", repr(diff))

#  MATRIX MULTIPLICATION METHOD
print("test : matrix multiplication method")

# Check multiplication method
prod = squareComplexMatrix1 * squareComplexMatrix4
print("prod = ", repr(prod))

# MULTIPLICATION WITH A NUMERICAL POINT METHOD
print("test : multiplication with a numerical point method")

# Create the numerical point
pt = ot.ComplexCollection(2)
pt[0] = 1.0 + 1j
pt[1] = 1j
print("pt = ", repr(pt))

# Check the product method
ptResult = squareComplexMatrix1 * pt
print("ptResult = ", repr(ptResult))

# MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
print("test : multiplication and division by a numerical scalar methods")

# Check the multiplication method
s = 3.0 + 2j
scalprod = squareComplexMatrix1 * s

print("scalprod = ", repr(scalprod))

# Check the division method
scaldiv1 = squareComplexMatrix1 / s
scaldiv2 = squareComplexMatrix1 / s
print("scaldiv1 = ", repr(scaldiv1))
print("scaldiv2 = ", repr(scaldiv2))
print("scaldiv1 equals scaldiv2 = ", (scaldiv1 == scaldiv2))

# ISEMPTY METHOD
print("test : isEmpty method")

# Check method isEmpty
squareComplexMatrix5 = ot.SquareComplexMatrix()
squareComplexMatrix6 = ot.SquareComplexMatrix()
print("squareComplexMatrix0 is empty = ", squareComplexMatrix0.isEmpty())
print("squareComplexMatrix1 is empty = ", squareComplexMatrix1.isEmpty())
print("squareComplexMatrix5 is empty = ", squareComplexMatrix5.isEmpty())

# Extra coverage: constructors, dimension, transpose variants, Hermitian ops
m_coll = ot.SquareComplexMatrix(2, [1 + 1j, 2 + 0j, 3 + 0j, 4 + 0j])
assert m_coll.getDimension() == 2
herm = ot.HermitianMatrix([[1.0, 0.5], [0.5, 2.0]])
m_herm = ot.SquareComplexMatrix(herm)
assert m_herm.getDimension() == 2
# transpose / conjugate / conjugateTranspose on 2x2
assert m_coll.transpose()[1, 0] == m_coll[0, 1]
assert m_coll.conjugateTranspose()[0, 1] == m_coll[1, 0].conjugate()
assert m_coll.conjugate()[0, 0] == m_coll[0, 0].conjugate()
# quick-return paths for empty and 1x1 matrices
assert ot.SquareComplexMatrix().transpose().getDimension() == 0
assert ot.SquareComplexMatrix(1, [2 + 3j]).transpose()[0, 0] == 2 + 3j
assert ot.SquareComplexMatrix(1, [2 + 3j]).conjugate()[0, 0] == 2 - 3j
assert ot.SquareComplexMatrix(1, [2 + 3j]).conjugateTranspose()[0, 0] == 2 - 3j
# operations with HermitianMatrix and IdentityMatrix
assert (m_coll + herm) - herm == m_coll
assert (m_coll - herm) + herm == m_coll
prod_h = m_coll * herm
assert prod_h.getNbRows() == 2 and prod_h.getNbColumns() == 2
assert m_coll * ot.IdentityMatrix(2) == m_coll
# products with vectors
assert list(m_coll * ot.Point([1.0, 2.0])) == list(m_coll * [1.0, 2.0])
assert list(m_coll * ot.ComplexCollection([1 + 0j, 2 + 0j])) == list(
    m_coll * ot.Point([1.0, 2.0])
)
