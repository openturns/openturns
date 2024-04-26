#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# for fft, the best implementation is given for N = 2^p
size = 16

# collection for test
collection = ot.ComplexCollection(size)

# Fill the data with artificial values

# Create a complex gaussian sample
for index in range(size):
    realPart = 0.1 * (index + 1.0) / size
    imagPart = 0.3 * (index + 1.0) / size
    collection[index] = realPart + 1j * imagPart

myFFT = ot.KissFFT()
print("myFFT = ", myFFT)

# Initial transformation
print("collection = ", collection)

# FFT transform
transformedCollection = ot.ComplexCollection(myFFT.transform(collection))
print("FFT result = ", transformedCollection)

# Inverse transformation
inverseTransformedCollection = ot.ComplexCollection(
    myFFT.inverseTransform(transformedCollection)
)
print("FFT back=", inverseTransformedCollection)

# 2D case now
N = 8
distribution = ot.Normal(N)
sample = distribution.getSample(2 * N)

# FFT transform
transformedSample = myFFT.transform2D(sample)
print("2D FFT result = ", repr(transformedSample))

# Inverse transformation
inverseTransformedSample = myFFT.inverseTransform2D(transformedSample)
print("2D FFT back=", repr(inverseTransformedSample.real()))

# 3D case
elements = [ot.RandomGenerator.Generate() for i in range(N * N * N)]
tensor = ot.ComplexTensor(N, N, N, elements)

# FFT transform
transformedTensor = myFFT.transform3D(tensor)
print("3D FFT result = ", repr(transformedTensor))

# Inverse transformation
inverseTransformedTensor = myFFT.inverseTransform3D(transformedTensor)
print("3D FFT back=", repr(inverseTransformedTensor.real()))
