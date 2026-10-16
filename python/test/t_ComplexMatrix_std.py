#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# DEFAULT CONSTRUCTOR AND STRING CONVERTER
print("test 0 : default constructor and string converter")

# Default constructor
matrix0 = ot.ComplexMatrix()

# String converter
print("matrix0 = ", repr(matrix0))

# CONSTRUCTOR WITH SIZE, OPERATOR() AND STRING CONVERTER
print("test number one : constructor with size, operator() and string converter")

# Constructor with size
matrix1 = ot.ComplexMatrix(2, 2)

# Check operator() methods
matrix1[0, 0] = 1.0 + 1j
matrix1[1, 0] = 2.0 + 4j
matrix1[0, 1] = 3.0 - 1j
matrix1[1, 1] = 4.0

# String converter
print("matrix1 = ", repr(matrix1))

# COPY CONSTRUCTOR AND STRING CONVERTER
print("test 2 : copy constructor and string converter")

# Copy constructor
matrix2 = ot.ComplexMatrix(matrix1)

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
elementsValues = ot.ComplexCollection()
elementsValues.add(1.0 - 1j)
elementsValues.add(2.0 - 1j)
elementsValues.add(3.0 - 1j)
elementsValues.add(4.0 + 1j)
elementsValues.add(5.0 + 1j)
elementsValues.add(6.0 + 1j)

# Check the content of the collection
print("elementsValues = ", repr(elementsValues))

# Check the constructor with collection
matrix0bis = ot.ComplexMatrix(2, 2, elementsValues)
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
pt = ot.Point()
pt.add(1.0)
pt.add(2.0)
print("pt = ", repr(pt))

# Check the product method
ptResult = matrix1 * pt
print("ptResult = ", repr(ptResult))

#  MULTIPLICATION AND DIVISION BY A NUMERICAL SCALAR METHODS
print("test 10 : multiplication and division by a numerical scalar methods")

# Check the multiplication method
s = 3.0 + 1j
scalprod1 = matrix1 * s
print("scalprod1 = ", repr(scalprod1))

# Check the division method
scaldiv1 = matrix1 / s
print("scaldiv1 = ", repr(scaldiv1))

#  ISEMPTY METHOD
print("test 10 : isEmpty method")

# Check method isEmpty
matrix7 = ot.ComplexMatrix()
matrix8 = ot.ComplexMatrix()

print("matrix1 is empty = ", matrix1.isEmpty())
print("matrix5 is empty = ", matrix7.isEmpty())
print("matrix6 is empty = ", matrix8.isEmpty())
print("matrix0 is empty = ", matrix0.isEmpty())

# MULTIPLICATION WITH A NUMERICAL POINT METHOD
print("test 11 : multiplication with a numerical point method")

# Create the numerical point
pt_test = ot.Point()
pt_test.add(1.0)
pt_test.add(2.0)
print("pt_test = ", repr(pt_test))

A = ot.ComplexMatrix(2, 2)
A[0, 0] = 0.5
A[1, 0] = -(m.sqrt(3.0) / 2)
A[0, 1] = m.sqrt(3.0) / 2
A[1, 1] = 0.5
B = A.transpose()
identity = B * A

# Check the product method
ptResult2 = identity * pt_test
print("A = ", repr(A))
print("B = ", repr(B))
print("identity = ", repr(identity))
print("ptResult2 = ", repr(ptResult2))

# Non-square matrix * vector
print("test 12 : non-square matrix multiplication with a point")
# Wide matrix (2 rows, 3 columns)
A = ot.ComplexMatrix(2, 3)
A[0, 0] = 1.0 + 2.0j
A[1, 0] = 3.0 + 4.0j
A[0, 1] = 5.0 + 6.0j
A[1, 1] = 7.0 + 8.0j
A[0, 2] = 9.0 + 0.0j
A[1, 2] = 10.0 + 0.0j
pt = ot.Point([1.0, 2.0, 3.0])
result = A * pt
print("wide A (2x3) * pt (3) = ", repr(result))
ptc = ot.ComplexCollection([1.0 + 0j, 2.0 + 0j, 3.0 + 0j])
resultc = A * ptc
print("wide A (2x3) * ptc (3) = ", repr(resultc))

# Tall matrix (3 rows, 2 columns)
B = ot.ComplexMatrix(3, 2)
B[0, 0] = 1.0 + 2.0j
B[1, 0] = 3.0 + 4.0j
B[2, 0] = 5.0 + 6.0j
B[0, 1] = 7.0 + 8.0j
B[1, 1] = 9.0 + 0.0j
B[2, 1] = 10.0 + 0.0j
pt2 = ot.Point([1.0, 2.0])
result2 = B * pt2
print("tall B (3x2) * pt (2) = ", repr(result2))
ptc2 = ot.ComplexCollection([1.0 + 0j, 2.0 + 0j])
resultc2 = B * ptc2
print("tall B (3x2) * ptc (2) = ", repr(resultc2))

# Edge case: 1x2 matrix
C = ot.ComplexMatrix(1, 2)
C[0, 0] = 1.0 + 0j
C[0, 1] = 2.0 + 0j
pt3 = ot.Point([3.0, 4.0])
result3 = C * pt3
print("narrow C (1x2) * pt (2) = ", repr(result3))

# Edge case: 2x1 matrix
D = ot.ComplexMatrix(2, 1)
D[0, 0] = 1.0 + 0j
D[1, 0] = 2.0 + 0j
pt4 = ot.Point([5.0])
result4 = D * pt4
print("narrow D (2x1) * pt (1) = ", repr(result4))

# --- Coverage: rectangular solve, clean/real/imag, vector products, error paths ---
A_cov = ot.ComplexMatrix(3, 2)
A_cov[0, 0] = 1 + 1j
A_cov[1, 0] = 2
A_cov[2, 0] = 3 - 1j
A_cov[0, 1] = 4
A_cov[1, 1] = 5 + 2j
A_cov[2, 1] = 6
B_cov = ot.ComplexMatrix(3, 2)
B_cov[0, 0] = 1
B_cov[1, 0] = 2
B_cov[2, 0] = 3
B_cov[0, 1] = 4
B_cov[1, 1] = 5
B_cov[2, 1] = 6
X_cov = A_cov.solveLinearSystem(B_cov)
assert X_cov.getNbRows() == 2 and X_cov.getNbColumns() == 2
x_cov = A_cov.solveLinearSystem(ot.ComplexCollection([1 + 0j, 2 + 0j, 3 + 0j]))
assert len(x_cov) == 2
m_cov = ot.ComplexMatrix(2, 2)
m_cov[0, 0] = 1.26 + 1.26j
m_cov[0, 1] = 0.04 + 0.04j
m_cov[1, 0] = 2
m_cov[1, 1] = 3
m_cleaned = m_cov.clean(0.1)
ott.assert_almost_equal(m_cleaned[0, 1], 0j, 1e-12, 1e-12)
ott.assert_almost_equal(m_cleaned.real()[0, 0], 1.3, 1e-12, 1e-12)
ott.assert_almost_equal(m_cleaned.imag()[0, 0], 1.3, 1e-12, 1e-12)
h_cov = ot.HermitianMatrix(2)
h_cov[0, 0] = 1
h_cov[1, 0] = 3 + 1j
h_cov[1, 1] = 4
assert h_cov.real().getNbRows() == 2
assert h_cov.imag().getNbRows() == 2
res_h_cc = h_cov * ot.ComplexCollection([1 + 1j, 2j])
assert abs(complex(res_h_cc[0]) - complex(3 + 7j)) < 1e-12
assert abs(complex(res_h_cc[1]) - complex(2 + 12j)) < 1e-12
res_h_pt = h_cov * ot.Point([1.0, 2.0])
assert abs(complex(res_h_pt[0]) - complex(7 - 2j)) < 1e-12
assert abs(complex(res_h_pt[1]) - complex(11 + 1j)) < 1e-12
t_cov = ot.TriangularComplexMatrix(2)
t_cov[0, 0] = 1
t_cov[1, 0] = 3 + 1j
t_cov[1, 1] = 4
res_t_pt = t_cov * ot.Point([1.0, 2.0])
assert abs(complex(res_t_pt[0]) - complex(1 + 0j)) < 1e-12
assert abs(complex(res_t_pt[1]) - complex(11 + 1j)) < 1e-12
res_t_cc = t_cov * ot.ComplexCollection([1 + 1j, 2j])
assert abs(complex(res_t_cc[0]) - complex(1 + 1j)) < 1e-12
assert abs(complex(res_t_cc[1]) - complex(2 + 12j)) < 1e-12
with ott.assert_raises(Exception):
    ot.ComplexMatrix(2, 2) * ot.ComplexMatrix(3, 3)
with ott.assert_raises(Exception):
    ot.HermitianMatrix(2) * ot.ComplexMatrix(3, 3)
with ott.assert_raises(Exception):
    ot.TriangularComplexMatrix(2) * ot.ComplexMatrix(3, 3)
with ott.assert_raises(Exception):
    ot.HermitianMatrix(2).computeCholesky()
str(m_cov)
