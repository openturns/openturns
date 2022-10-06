#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# We create a Function
myFunction = ot.SymbolicFunction(['x1', 'x2', 'x3', 'x4'], [
    '(x1*x1+x2^3*x1)/(2*x3*x3+x4^4+1)', 'cos(x2*x2+x4)/(x1*x1+1+x3^4)'])

# We create a distribution
dim = myFunction.getInputDimension()
R = ot.CorrelationMatrix(dim)
for i in range(dim):
    R[i, i] = 1.0
for i in range(1, dim):
    R[i, i - 1] = 0.5

m = ot.Point(dim, 1.0)
s = ot.Point(dim, 2.0)
distribution = ot.Normal(m, s, R)
ref_distribution = distribution
print("distribution = ", repr(ref_distribution))

# We create a distribution-based RandomVector
X = ot.RandomVector(distribution)
print("X=", X)
print("is composite? ", X.isComposite())

# Check standard methods of class RandomVector
print("X dimension=", X.getDimension())
print("X realization (first )=", repr(X.getRealization()))
print("X realization (second)=", repr(X.getRealization()))
print("X realization (third )=", repr(X.getRealization()))
print("X sample =", repr(X.getSample(5)))

# We create a composite RandomVector Y from X and myFunction
Y = ot.RandomVector(ot.CompositeRandomVector(myFunction, X))
print("Y=", Y)
print("is composite? ", Y.isComposite())

# Check standard methods of class RandomVector
print("Y dimension=", Y.getDimension())
print("Y realization (first )=", repr(Y.getRealization()))
print("Y realization (second)=", repr(Y.getRealization()))
print("Y realization (third )=", repr(Y.getRealization()))
print("Y sample =", repr(Y.getSample(5)))
parameter = Y.getParameter()
print("Y parameter =", repr(parameter))
Y.setParameter(parameter)
print("Y parameter desc =", repr(Y.getParameterDescription()))
