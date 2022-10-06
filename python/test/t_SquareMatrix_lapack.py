#! /usr/bin/env python

import openturns as ot
ot.TESTPREAMBLE()


matrix1 = ot.SquareMatrix(2)
matrix1.setName("matrix1")
matrix1[0, 0] = 1.0
matrix1[1, 0] = 2.0
matrix1[0, 1] = 5.0
matrix1[1, 1] = 12.0
print("matrix1 = ", matrix1)

pt = ot.Point()
pt.add(5.0)
pt.add(0.0)
print("pt = ", pt)

result = matrix1.solveLinearSystem(pt)
print("result = ", result)

determinant = matrix1.computeDeterminant()
print("determinant = %.6g" % determinant)

ev = matrix1.computeEigenValues()
print("ev = ", ev)
ev, evect = matrix1.computeEV()
print("ev=", ev)
print("evect=", evect)
print("evect=")
print(evect.__str__())
maxModule = matrix1.computeLargestEigenValueModule(10, 1e-2)
print("max |ev|=%.6g" % maxModule)

# Check the high dimension determinant computation
matrix2 = ot.SquareMatrix(3)
matrix2[0, 0] = 1.0
matrix2[0, 1] = 2.0
matrix2[0, 2] = 3.0
matrix2[1, 0] = -1.5
matrix2[1, 1] = 2.5
matrix2[1, 2] = -3.5
matrix2[2, 0] = 1.5
matrix2[2, 1] = -3.5
matrix2[2, 2] = 2.5

print("matrix2=")
print(matrix2.__str__())
# Need a specific Python wrapping, e.g returning both value and sign
# sign = 0.0
# value = matrix2.computeLogAbsoluteDeterminant(sign)
# print "log(|det|)=", value, ", sign=", sign
determinant = matrix2.computeDeterminant()
print("determinant = %.6g" % determinant)
