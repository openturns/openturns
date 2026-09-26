#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(("E", "F", "L", "I"), ("-F*L^3/(3.*E*I)",))

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [50.0, 1.0, 10.0, 5.0]
sigma = ot.Point(dim, 1.0)
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -3)

# We create a FORM algorithm
myCobyla = ot.Cobyla()
myCobyla.setMaximumCallsNumber(400)
myCobyla.setStartingPoint(mean)
myAlgo = ot.FORM(myCobyla, myEvent)

# Perform the simulation
myAlgo.run()

# Create a PostAnalyticalImportanceSampling algorithm based on
# the previous FORM result
formResult = myAlgo.getResult()
mySamplingAlgo = ot.PostAnalyticalImportanceSampling(formResult)
print("FORM probability= %.11g" % formResult.getEventProbability())
mySamplingAlgo.setMaximumOuterSampling(250)
mySamplingAlgo.setBlockSize(4)
mySamplingAlgo.setMaximumCoefficientOfVariation(0.1)

print("PostAnalyticalImportanceSampling=", mySamplingAlgo)

mySamplingAlgo.run()

# Stream out the result
print("PostAnalyticalImportanceSampling result=", mySamplingAlgo.getResult())

# Ticket #3210: Strange differences between PostAnalyticalControlledImportanceSampling and PostAnalyticalImportanceSampling

model = ot.SymbolicFunction(["u"], ["u"])

inputDist = ot.Uniform(0.0, 1.0)
inputVector = ot.RandomVector(inputDist)

Y = ot.CompositeRandomVector(model, inputVector)

thresholds = [0.05 + 0.1 * i for i in range(10)]

for i, comp in enumerate([ot.Less(), ot.Greater()]):
    for threshold in thresholds:
        event = ot.ThresholdEvent(Y, comp, threshold)

        solver = ot.Cobyla()
        solver.setStartingPoint(inputDist.getMean())
        solver.setMaximumConstraintError(1e-4)
        algo = ot.FORM(solver, event)
        algo.run()
        result_form = algo.getResult()

        algo = ot.PostAnalyticalImportanceSampling(result_form)
        algo.setMaximumOuterSampling(1000)
        algo.setBlockSize(100)
        algo.setMaximumCoefficientOfVariation(0.001)
        algo.run()
        if i == 0:
            ref = threshold
        else:
            ref = 1.0 - threshold
        ott.assert_almost_equal(algo.getResult().getProbabilityEstimate(), ref, 5e-2, 5e-2)
