#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Default dimension parameter to evaluate the model
defaultDimension = 1
inputDimension = 1

# Amplitude values
amplitude = ot.Point(defaultDimension, 2.0)

# Scale values
scale = ot.Point(inputDimension, 1.0)

# Default constructor
myDefaultModel = ot.ExponentialModel()
print("myDefaultModel = ", myDefaultModel)

# Second order model with parameters
myModel = ot.ExponentialModel(scale, amplitude)
print("myModel = ", myModel)

timeValueOne = 1.0
print("covariance matrix at t = ", timeValueOne, " : ", myModel(timeValueOne))
print(
    "covariance matrix at t = ",
    -1.0 * timeValueOne,
    " : ",
    myModel(-1.0 * timeValueOne),
)

# Evaluation at time higher to check the decrease of the exponential values
timeValueHigh = 15.0
print(
    "covariance matrix at t = ", timeValueHigh, " : ", myModel(timeValueHigh).__str__()
)

timeGrid = ot.RegularGrid(0.0, 1.0 / 3.0, 4)
print(
    "discretized covariance over the time grid=",
    timeGrid,
    "is=",
    myModel.discretize(timeGrid),
)

# Default dimension parameter to evaluate the model
highDimension = 3

# Reallocation of adequate sizes
amplitude.resize(highDimension)
spatialCorrelation = ot.CorrelationMatrix(highDimension)
for index in range(highDimension):
    amplitude[index] = (index + 1) / (defaultDimension * defaultDimension)
    if index > 0:
        spatialCorrelation[index, index - 1] = 1.0 / (index * index)

# Second order model  - dimension 10
myHighModel = ot.ExponentialModel(scale, amplitude, spatialCorrelation)
print("myHighModel = ", myHighModel)

print("covariance matrix at t = ", timeValueOne, " : ", myHighModel(timeValueOne))
print(
    "covariance matrix at t = ",
    -1.0 * timeValueOne,
    " : ",
    myHighModel(-1.0 * timeValueOne),
)
print("covariance matrix at t = ", timeValueHigh, " : ", myHighModel(timeValueHigh))

print(
    "discretized covariance over the time grid=",
    timeGrid,
    "is=",
    myHighModel.discretize(timeGrid),
)

marginal = myHighModel.getMarginal([0, 2])
print("parameters=", myHighModel.getParameter(), myHighModel.getParameterDescription())
print(
    "marginal=",
    marginal,
    "marginal.parameter=",
    marginal.getParameter(),
    marginal.getParameterDescription(),
)

# parameter bug
model = ot.ExponentialModel([1.0] * 3, [2.0] * 2)
model.setActiveParameter(range(6))
p = model.getParameter()
print(p)
p[-1] = 0.5
model.setParameter(p)
p = model.getParameter()
print(p)

# draw bug
graph = ot.ExponentialModel().draw()
print("ok")
