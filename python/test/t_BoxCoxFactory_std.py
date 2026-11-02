#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# TimeGrid parameters
n = 101
timeStart = 0.0
timeStep = 0.1
timeGrid = ot.RegularGrid(timeStart, timeStep, n)

# White noise
whiteNoise = ot.WhiteNoise(ot.Uniform(), timeGrid)

# Composite process
process = ot.CompositeProcess(
    ot.ValueFunction(ot.SymbolicFunction("x", "x+2"), timeGrid), whiteNoise
)
# A realization of the process
timeSeries = process.getRealization()
sample = timeSeries.getValues()

# Now we build the factory
factory = ot.BoxCoxFactory()

# Creation of the BoxCoxTransform
myBoxCox = factory.build(timeSeries)

print("myBoxCox (time-series)=", myBoxCox)
print("myBoxCox (sample)     =", factory.build(sample))

# Creation of the BoxCoxTransform using shift
shift = ot.Point(1, 1.0)
myBoxCoxShift, graph = factory.buildWithGraph(timeSeries, shift)

print("myBoxCox with shift (time-series)=", myBoxCoxShift)
print("myBoxCox with shift (sample)     =", factory.build(sample, shift))
print("BoxCox graph (time-series)=", graph)

# with sample
myBoxCoxShift, graph = factory.buildWithGraph(sample, shift)

# Lambda profile-likelihood confidence interval, see issue #2849
ciSample = ot.LogNormal().getSample(1000)
ci = factory.computeLambdaConfidenceInterval(ciSample)
lambdaHat = factory.build(ciSample).getLambda()
assert ci.getLowerBound()[0] <= lambdaHat[0] <= ci.getUpperBound()[0]
ci50 = factory.computeLambdaConfidenceInterval(ciSample, 0.5)
assert (ci50.getUpperBound()[0] - ci50.getLowerBound()[0]) <= (
    ci.getUpperBound()[0] - ci.getLowerBound()[0]
)
for badLevel in [0.0, 1.0, 1.5]:
    with ott.assert_raises(TypeError):
        factory.computeLambdaConfidenceInterval(ciSample, badLevel)
ci2d = factory.computeLambdaConfidenceInterval(
    ot.JointDistribution([ot.Uniform(1.0, 2.0)] * 2).getSample(200)
)
assert ci2d.getDimension() == 2
# The search bracket must expand beyond [-2, 2] when the level is not
# crossed there, as for a two-point sample
ciSmall = factory.computeLambdaConfidenceInterval(ot.Sample([[1.0], [2.0]]))
assert ciSmall.getLowerBound()[0] < -2.0
assert ciSmall.getUpperBound()[0] > 2.0
assert ciSmall.getFiniteLowerBound()[0]
assert ciSmall.getFiniteUpperBound()[0]
