#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Default dimension parameter to evaluate the model
defaultDimension = 1

# Amplitude values
amplitude = ot.Point(defaultDimension, 1.0)

# Scale values
scale = ot.Point(defaultDimension, 1.0)

# Default constructor
myDefaultModel = ot.CauchyModel()
print("myDefaultModel = ", myDefaultModel)

# Second order model with parameters
myModel = ot.CauchyModel(scale, amplitude)
print("myModel = ", myModel)

frequencyValue = 1.0
print("spectral density matrix at f = ", frequencyValue, " : ", myModel(frequencyValue))

# Evaluation at time higher to check the decrease of the cauchy values
frequencyValueHigh = 10.0
print(
    "spectral density matrix at f = ",
    frequencyValueHigh,
    " : ",
    myModel(frequencyValueHigh),
)

# Default dimension parameter to evaluate the model
highDimension = 3

# Reallocation of adequate sizes
amplitude.resize(highDimension)
scale.resize(highDimension)
spatialCorrelation = ot.CorrelationMatrix(highDimension)
for index in range(highDimension):
    amplitude[index] = 1.0
    scale[index] = (index + 1.0) / (defaultDimension * defaultDimension)
    if index > 0:
        spatialCorrelation[index, index - 1] = 1.0 / (index * index)

# check the cast
mySecondOrderModel = ot.SpectralModel(
    ot.CauchyModel(scale, amplitude, spatialCorrelation)
)
print("mySecondOrderModel = ", mySecondOrderModel)
# checking the cast

# Second order model  - dimension 10
myHighModel = ot.CauchyModel(scale, amplitude, spatialCorrelation)
print("myHighModel = ", myHighModel)
print("spectral density matrix at f = ", frequencyValue, " : ", myModel(frequencyValue))
print(
    "spectral density matrix at f = ",
    frequencyValueHigh,
    " : ",
    myModel(frequencyValueHigh),
)
