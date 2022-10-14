#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create a distribution
meanPoint = ot.Point(1)
meanPoint[0] = 1.0
sigma = ot.Point(1)
sigma[0] = 1.0
R = ot.CorrelationMatrix(1)
distribution = ot.Normal(meanPoint, sigma, R)

ref_distribution = distribution

print("distribution = ", repr(ref_distribution))

# We create a distribution-based RandomVector
vect = ot.RandomVector(distribution)
print("vect=", vect)

# Check standard methods of class RandomVector
print("vect dimension=", vect.getDimension())
print("vect realization (first )=", repr(vect.getRealization()))
print("vect realization (second)=", repr(vect.getRealization()))
print("vect realization (third )=", repr(vect.getRealization()))
print("vect sample =", repr(vect.getSample(5)))
parameter = vect.getParameter()
print("vect parameter =", repr(parameter))
vect.setParameter(parameter)
print("vect parameter desc =", repr(vect.getParameterDescription()))
