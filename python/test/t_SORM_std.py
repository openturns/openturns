#! /usr/bin/env python

import openturns as ot


ot.TESTPREAMBLE()


# We create a numerical math function
myFunction = ot.SymbolicFunction(["E", "F", "L", "I"], ["-F*L^3/(3*E*I)"])

dim = myFunction.getInputDimension()

# We create a normal distribution point of dimension 1
mean = [50.0, 1.0, 10.0, 5.0]
sigma = [1.0] * dim
R = ot.IdentityMatrix(dim)
myDistribution = ot.Normal(mean, sigma, R)

# We create a 'usual' RandomVector from the Distribution
vect = ot.RandomVector(myDistribution)

# We create a composite random vector
output = ot.CompositeRandomVector(myFunction, vect)

# We create an Event from this RandomVector
myEvent = ot.ThresholdEvent(output, ot.Less(), -1.5)

# We create a NearestPoint algorithm
myAbdoRackwitz = ot.AbdoRackwitz()
myAbdoRackwitz.setStartingPoint(mean)
myAbdoRackwitz.setMaximumIterationNumber(100)
myAbdoRackwitz.setMaximumAbsoluteError(1.0e-10)
myAbdoRackwitz.setMaximumRelativeError(1.0e-10)
myAbdoRackwitz.setMaximumResidualError(1.0e-10)
myAbdoRackwitz.setMaximumConstraintError(1.0e-10)
print("myAbdoRackwitz=", myAbdoRackwitz)

# We create a FORM algorithm
# The first parameter is an OptimizationAlgorithm
# The second parameter is an event
# The third parameter is a starting point for the design point research
myAlgo = ot.SORM(myAbdoRackwitz, myEvent)

# Perform the simulation
myAlgo.run()

# Stream out the result
result = myAlgo.getResult()
digits = 5
print("Breitung event probability=%.6f" % result.getEventProbabilityBreitung())
print(
    "Breitung generalized reliability index=%.6f"
    % result.getGeneralisedReliabilityIndexBreitung()
)
print("Hohenbichler event probability=%.6f" % result.getEventProbabilityHohenbichler())
print(
    "Hohenbichler generalized reliability index=%.6f"
    % result.getGeneralisedReliabilityIndexHohenbichler()
)
print("Tvedt event probability=%.6f" % result.getEventProbabilityTvedt())
print(
    "Tvedt generalized reliability index=%.6f"
    % result.getGeneralisedReliabilityIndexTvedt()
)
curvatures = result.getSortedCurvatures()
curvatures[2] = 0.0
print("sorted curvatures=", curvatures)
print("standard space design point=", result.getStandardSpaceDesignPoint())
print("physical space design point=", result.getPhysicalSpaceDesignPoint())

# Is the standard point origin in failure space?
print(
    "is standard point origin in failure space? ",
    result.getIsStandardPointOriginInFailureSpace(),
)

print("importance factors=", result.getImportanceFactors())
print("Hasofer reliability index=%.6f" % result.getHasoferReliabilityIndex())
