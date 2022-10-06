#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# We create a numerical math function
myFunction = ot.SymbolicFunction(
    ('E', 'F', 'L', 'I'), ('-F*L^3/(3.*E*I)',))

dim = myFunction.getInputDimension()
# We create a normal distribution point of dimension 1
mean = ot.Point(dim, 0.0)
# E
mean[0] = 50.0
# F
mean[1] = 1.0
# L
mean[2] = 10.0
# I
mean[3] = 5.0
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector */
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector */
myEvent = ot.ThresholdEvent(output, ot.Less(), -3)

# TEST ONE : directional sampling from an event (slow and safe strategy by default)
# We create a Directional Sampling algorithm */
myAlgo = ot.DirectionalSampling(myEvent)
myAlgo.setMaximumOuterSampling(250)
myAlgo.setBlockSize(4)
myAlgo.setMaximumCoefficientOfVariation(0.1)

print("DirectionalSampling=", myAlgo)

# Perform the simulation */
myAlgo.run()

# Stream out the result */
print("DirectionalSampling result=", myAlgo.getResult())

# TEST TWO : diectional sampling from an event and a root strategy : Risky And Fast, and a directional sampling strategy :
# We create a Directional Sampling algorithm */
myAlgo2 = ot.DirectionalSampling(myEvent, ot.MediumSafe(), ot.OrthogonalDirection())
myAlgo2.setMaximumOuterSampling(250)
myAlgo2.setBlockSize(4)
myAlgo2.setMaximumCoefficientOfVariation(0.1)

print("DirectionalSampling=", myAlgo2)

# Perform the simulation */
myAlgo2.run()

# Stream out the result */
print("DirectionalSampling result=", myAlgo2.getResult())

# Ticket #778
myFunction = ot.SymbolicFunction('x', 'x')

X1 = ot.RandomVector(ot.Uniform(-2, 1.99999))
X2 = ot.RandomVector(ot.Uniform(-2, 2.0))

for X in [X1, X2]:
    vector = ot.CompositeRandomVector(myFunction, X)
    event = ot.ThresholdEvent(vector, ot.GreaterOrEqual(), 0.0)

    print('X:', X.getDistribution())

    myAlgo3 = ot.DirectionalSampling(event)
    n1 = myFunction.getCallsNumber()
    myAlgo3.run()
    n2 = myFunction.getCallsNumber()
    result = myAlgo3.getResult().getProbabilityEstimate()
    print('p=%.6g (ncalls = %d)' % (result, n2 - n1))
