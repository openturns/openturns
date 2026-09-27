#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

myFFT = ot.KissFFT()
print("myFFT = ", myFFT)

# 1D case with deterministic data
size = 16
collection = ot.ComplexCollection(size)
for index in range(size):
    realPart = 0.1 * (index + 1.0) / size
    imagPart = 0.3 * (index + 1.0) / size
    collection[index] = realPart + 1j * imagPart

print("collection = ", collection)

transformedCollection = ot.ComplexCollection(myFFT.transform(collection))
print("FFT result = ", transformedCollection)

inverseCollection = ot.ComplexCollection(
    myFFT.inverseTransform(transformedCollection)
)
print("FFT back=", inverseCollection)

# Verify 1D roundtrip
for i in range(size):
    ott.assert_almost_equal(inverseCollection[i].real, collection[i].real, 1e-14, 1e-13)
    ott.assert_almost_equal(inverseCollection[i].imag, collection[i].imag, 1e-14, 1e-13)

# 2D case
N = 8
ot.RandomGenerator.SetSeed(0)
distribution = ot.Normal(N)
sample = distribution.getSample(2 * N)
rows = sample.getSize()
cols = sample.getDimension()
sampleFlat = ot.ComplexCollection(
    [complex(sample[i, j], 0.0) for j in range(cols) for i in range(rows)]
)

transformedSample = myFFT.transform(sampleFlat, [rows, cols])
print("2D FFT size = ", transformedSample.getSize())

inverseSample = myFFT.inverseTransform(transformedSample, [rows, cols])
print("2D FFT back size = ", inverseSample.getSize())

for i in range(rows * cols):
    ott.assert_almost_equal(inverseSample[i].real, sampleFlat[i].real, 1e-12, 1e-12)
    ott.assert_almost_equal(inverseSample[i].imag, sampleFlat[i].imag, 0.0, 1e-12)

# 3D case
ot.RandomGenerator.SetSeed(0)
elements = [ot.RandomGenerator.Generate() for i in range(N * N * N)]
tensorFlat = ot.ComplexCollection(
    [complex(elements[i + j * N + k * N * N], 0.0)
     for k in range(N) for j in range(N) for i in range(N)]
)

transformedTensor = myFFT.transform(tensorFlat, [N, N, N])
print("3D FFT size = ", transformedTensor.getSize())

inverseTensor = myFFT.inverseTransform(transformedTensor, [N, N, N])
print("3D FFT back size = ", inverseTensor.getSize())

for i in range(N * N * N):
    ott.assert_almost_equal(inverseTensor[i].real, tensorFlat[i].real, 1e-12, 1e-12)
    ott.assert_almost_equal(inverseTensor[i].imag, tensorFlat[i].imag, 0.0, 1e-12)
