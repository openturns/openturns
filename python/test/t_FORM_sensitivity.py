#! /usr/bin/env python

import openturns as ot


ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(["x0", "x1"], ["-(6+x0^2-x1)"])

dim = myFunction.getInputDimension()
# We create a normal distribution point of dimension 1
mean = [5.0, 2.1]
sigma = [3.3, 3.0]
R = ot.IdentityMatrix(dim)

#
testDistributions = ot.DistributionCollection(2)
testDistributions[0] = ot.Normal(mean, sigma, R)
marginals = ot.DistributionCollection(2)
marginals[0] = testDistributions[0].getMarginal(0)
marginals[1] = testDistributions[0].getMarginal(1)
testDistributions[1] = ot.JointDistribution(marginals, ot.NormalCopula(R))
for i in range(1):
    myDistribution = testDistributions[i]
    myDistribution.setDescription(["Marginal 1", "Marginal 2"])

    # We create a 'usual' RandomVector from the Distribution
    vect = ot.RandomVector(myDistribution)

    # We create a composite random vector
    output = ot.CompositeRandomVector(myFunction, vect)
    output.setDescription(["Interest Variable 1"])

    # We create an Event from this RandomVector
    myEvent = ot.ThresholdEvent(output, ot.Greater(), 0.0)

    # We create a NearestPoint algorithm
    myCobyla = ot.Cobyla()
    myCobyla.setStartingPoint(mean)
    myCobyla.setMaximumCallsNumber(200)
    myCobyla.setMaximumAbsoluteError(1.0e-10)
    myCobyla.setMaximumRelativeError(1.0e-10)
    myCobyla.setMaximumResidualError(1.0e-10)
    myCobyla.setMaximumConstraintError(1.0e-10)
    print("myCobyla=", myCobyla)

    # We create a FORM algorithm
    # The first parameter is an OptimizationAlgorithm
    # The second parameter is an event
    # The third parameter is a starting point for the design point research
    myAlgo = ot.FORM(myCobyla, myEvent)

    print("FORM=", myAlgo)

    # Perform the simulation
    myAlgo.run()

    # Stream out the result
    result = ot.FORMResult(myAlgo.getResult())
    print("importance factors=", result.getImportanceFactors())
    print("Hasofer reliability index=%.6f" % result.getHasoferReliabilityIndex())
    print("result=", result)

    # Hasofer Reliability Index Sensitivity
    hasoferReliabilityIndexSensitivity = result.getHasoferReliabilityIndexSensitivity()
    print(
        "hasoferReliabilityIndexSensitivity = ",
        repr(hasoferReliabilityIndexSensitivity),
    )

    # Event Probability Sensitivity
    eventProbabilitySensitivity = result.getEventProbabilitySensitivity()
    print("eventProbabilitySensitivity = ", repr(eventProbabilitySensitivity))
