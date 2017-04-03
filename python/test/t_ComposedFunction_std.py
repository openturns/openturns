#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

# Left hand side of the composition
left = ot.SymbolicFunction(
    ['x1', 'x2'], ['x1*sin(x2)', 'cos(x1+x2)', '(x2+1)*exp(x1-2*x2)'])

# Right hand side of the composition
right = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4'], [
                            '(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)', 'cos(x2*x2+x4)/(x1*x1+1+x3^4)'])

# Compositon of left and right
composed = ot.ComposedFunction(left, right)

print("right=", repr(right))
print("left=", repr(left))
print("composed=", repr(composed))

# Does it work?
x = ot.Point(right.getInputDimension(), 1.0)
y = right(x)
z = left(y)
Dy = right.gradient(x)
Dz = left.gradient(y)

print("x=", repr(x), " y=right(x)=", repr(y), " z=left(y)=", repr(z))
print("left(right(x))=", repr(composed(x)))
print("D(right)(x)=", repr(Dy), " D(left)(y)=", repr(Dz))
print(" prod=", repr(Dy * Dz))
print("D(left(right(x)))=", repr(composed.gradient(x)))
result = composed.hessian(x)
print("DD(left(right(x)))=")
for k in range(result.getNbSheets()):
    for j in range(result.getNbColumns()):
        for i in range(result.getNbRows()):
            print("%.6f" % result[i, j, k])
        print("")
    print("")
