#! /usr/bin/env python

from __future__ import print_function
from openturns import *
import math

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    # for fft, the best implementation is given for N = 2^p
    size = 16

    # collection for test
    collection = ComplexCollection(size)

    # Fill the data with artificial values

    # Create a complex gaussian sample
    for index in range(size):
        realPart = 0.1 * (index + 1.0) / size
        imagPart = 0.3 * (index + 1.0) / size
        collection[index] = realPart + 1j * imagPart

    # Instanciation of FFT class
    myFFT = KissFFT()
    print("myFFT = ", myFFT)

    # Initial transformation
    print("collection = ", collection)

    # FFT transform
    transformedCollection = ComplexCollection(
        myFFT.transform(collection))
    print("FFT result = ", transformedCollection)

    # Inverse transformation
    inverseTransformedCollection = ComplexCollection(
        myFFT.inverseTransform(transformedCollection))
    print("FFT back=", inverseTransformedCollection)

    # 2D case now
    N = 8
    distribution = Normal(N)
    sample = distribution.getSample(2 * N)

    # cleaning parameter
    threshold = 1e-14
    # FFT transform
    transformedSample = myFFT.transform2D(sample)
    print("2D FFT result = ", repr(transformedSample.clean(threshold)))

    # Inverse transformation
    inverseTransformedSample = myFFT.inverseTransform2D(transformedSample)
    print("2D FFT back=", repr(inverseTransformedSample.clean(threshold)))

    # 3D case
    elements = [RandomGenerator.Generate() for i in range(N * N * N)]
    tensor = ComplexTensor(N, N, N, elements)

    # FFT transform
    transformedTensor = myFFT.transform3D(tensor)
    print("3D FFT result = ", repr(transformedTensor.clean(threshold)))

    # Inverse transformation
    inverseTransformedTensor = myFFT.inverseTransform3D(transformedTensor)
    print("3D FFT back=", repr(inverseTransformedTensor.clean(threshold)))

except:
    import sys
    print("t_KissFFT_std.py", sys.exc_info()[0], sys.exc_info()[1])
