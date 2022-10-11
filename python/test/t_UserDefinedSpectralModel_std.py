#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


def cleanScalar(inScalar):
    if abs(inScalar) < 1.0e-6:
        return 0.0
    return inScalar


# Default constructor
myDefaultModel = ot.UserDefinedSpectralModel()
print("myDefaultModel = ", myDefaultModel)

# Default dimension parameter to evaluate the model
dimension = 2
inputDimension = 1

# Amplitude values
amplitude = ot.Point(dimension)
# Scale values
scale = ot.Point(inputDimension, 1.0)
# Spatial correlation
spatialCorrelation = ot.CorrelationMatrix(dimension)
for index in range(dimension):
    # constant amplitude
    amplitude[index] = (index + 1.0) / dimension
    if index > 0:
        spatialCorrelation[index, index - 1] = 1.0 / index

# Sample a CauchyModel
referenceModel = ot.CauchyModel(scale, amplitude, spatialCorrelation)

size = 5
frequencyGrid = ot.RegularGrid(0.0, 2.0 / size, size)
dspCollection = ot.HermitianMatrixCollection(frequencyGrid.getN())
for i in range(frequencyGrid.getN()):
    dspCollection[i] = referenceModel(frequencyGrid.getValue(i))

# Create a UserDefinedSpectralModel
myModel = ot.UserDefinedSpectralModel(frequencyGrid, dspCollection)
print("myModel=", myModel)

# Sample the UserDefinedSpectralModel
samplingGrid = ot.RegularGrid(-0.4, 1.0 / 16, 5 * size)

for i in range(samplingGrid.getN()):
    frequency = samplingGrid.getValue(i)
    print("frequency= %g myModel=" % cleanScalar(frequency))
    print(myModel(frequency).clean(1e-6), ", referenceModel=")
    print(referenceModel(frequency).clean(1e-6))
