#! /usr/bin/env python

import os

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

fft = otexp.PocketFFT()
print("fft = ", fft)

# 1D case with deterministic data
size = 16
collection = ot.ComplexCollection(size)
for index in range(size):
    realPart = 0.1 * (index + 1.0) / size
    imagPart = 0.3 * (index + 1.0) / size
    collection[index] = realPart + 1j * imagPart

transformedCollection = ot.ComplexCollection(fft.transform(collection))
print("FFT result size = ", transformedCollection.getSize())

inverseCollection = ot.ComplexCollection(fft.inverseTransform(transformedCollection))
for i in range(size):
    ott.assert_almost_equal(inverseCollection[i].real, collection[i].real, 1e-14, 1e-13)
    ott.assert_almost_equal(inverseCollection[i].imag, collection[i].imag, 1e-14, 1e-13)
print("1D roundtrip OK")

# The transform of a sum of sequences is the sum of their transforms
other = ot.ComplexCollection([complex(i + 1.0, 2.0 * i) for i in range(size)])
sumCollection = ot.ComplexCollection(
    [complex(collection[i].real + other[i].real, collection[i].imag + other[i].imag)
     for i in range(size)]
)
transformOfSum = ot.ComplexCollection(fft.transform(sumCollection))
sumOfTransforms = ot.ComplexCollection(fft.transform(collection))
sumOfTransforms2 = ot.ComplexCollection(fft.transform(other))
for i in range(size):
    referenceReal = sumOfTransforms[i].real + sumOfTransforms2[i].real
    referenceImag = sumOfTransforms[i].imag + sumOfTransforms2[i].imag
    ott.assert_almost_equal(transformOfSum[i].real, referenceReal, 1e-14, 1e-13)
    ott.assert_almost_equal(transformOfSum[i].imag, referenceImag, 1e-14, 1e-13)
print("1D linearity OK")

# The transform of a constant sequence is a Dirac at the origin
constantCollection = ot.ComplexCollection([complex(2.0, 0.0) for i in range(8)])
constantTransform = ot.ComplexCollection(fft.transform(constantCollection))
ott.assert_almost_equal(constantTransform[0].real, 16.0, 1e-14, 0.0)
for i in range(1, 8):
    ott.assert_almost_equal(constantTransform[i].real, 0.0, 0.0, 1e-13)
    ott.assert_almost_equal(constantTransform[i].imag, 0.0, 0.0, 1e-13)
print("constant sequence OK")

# Round-trips at prime sizes, including size 101 handled by Bluestein's
# chirp-z algorithm instead of the O(N^2) direct transform of the kissfft
# back-end
for primeSize in [13, 101]:
    primeCollection = ot.ComplexCollection(
        [complex(i + 1.0, 0.0) for i in range(primeSize)]
    )
    primeTransform = ot.ComplexCollection(fft.transform(primeCollection))
    primeRestored = ot.ComplexCollection(fft.inverseTransform(primeTransform))
    for i in range(primeSize):
        ott.assert_almost_equal(primeRestored[i].real, primeCollection[i].real, 1e-12, 1e-12)
        ott.assert_almost_equal(primeRestored[i].imag, 0.0, 0.0, 1e-12)
print("prime size roundtrip OK")

# Block transform on a part of the collection, matching the full transform of
# the extracted block
blockFirst = 4
blockSize = 8
blockExtracted = ot.ComplexCollection(
    [collection[blockFirst + i] for i in range(blockSize)]
)
blockTransform = ot.ComplexCollection(fft.transform(collection, blockFirst, blockSize))
blockReference = ot.ComplexCollection(fft.transform(blockExtracted))
for i in range(blockSize):
    ott.assert_almost_equal(blockTransform[i].real, blockReference[i].real, 1e-14, 1e-13)
    ott.assert_almost_equal(blockTransform[i].imag, blockReference[i].imag, 1e-14, 1e-13)
blockInverse = ot.ComplexCollection(
    fft.inverseTransform(collection, blockFirst, blockSize)
)
blockInverseReference = ot.ComplexCollection(fft.inverseTransform(blockExtracted))
for i in range(blockSize):
    ott.assert_almost_equal(blockInverse[i].real, blockInverseReference[i].real, 1e-14, 1e-13)
    ott.assert_almost_equal(blockInverse[i].imag, blockInverseReference[i].imag, 1e-14, 1e-13)
print("block transform OK")

# Out of range block arguments raise
with ott.assert_raises(TypeError):
    fft.transform(collection, size, 1)
with ott.assert_raises(TypeError):
    fft.transform(collection, 1, size)
with ott.assert_raises(TypeError):
    fft.inverseTransform(collection, size, 1)
with ott.assert_raises(TypeError):
    fft.inverseTransform(collection, 1, size)
print("block out of bounds errors OK")

# An empty collection is transformed into an empty collection
emptyTransform = ot.ComplexCollection(fft.transform(ot.ComplexCollection(0)))
emptyInverse = ot.ComplexCollection(fft.inverseTransform(ot.ComplexCollection(0)))
print("empty collection sizes = ", emptyTransform.getSize(), emptyInverse.getSize())

# An empty shape is the identity
identity = ot.ComplexCollection([complex(3.0, 4.0)])
identityResult = fft.transform(identity, ot.Indices())
ott.assert_almost_equal(identityResult[0].real, 3.0, 1e-14, 0.0)
ott.assert_almost_equal(identityResult[0].imag, 4.0, 1e-14, 0.0)
print("empty shape identity OK")

# The real input path converts the sequence to complex with a zero imaginary
# part and must agree with the complex input path
data2D = ot.Point([float(i + 1) for i in range(6)])
complex2D = ot.ComplexCollection([complex(i + 1.0, 0.0) for i in range(6)])
transform2D = fft.transform(data2D, [2, 3])
transform2DComplex = fft.transform(complex2D, [2, 3])
for i in range(6):
    ott.assert_almost_equal(transform2D[i].real, transform2DComplex[i].real, 1e-14, 0.0)
    ott.assert_almost_equal(transform2D[i].imag, transform2DComplex[i].imag, 1e-14, 0.0)
print("real and complex input agree OK")

# 1D case with a single dimension
data1D = ot.Point([float(i + 1) for i in range(4)])
complex1D = ot.ComplexCollection([complex(i + 1.0, 0.0) for i in range(4)])
transform1D = fft.transform(data1D, [4])
reference1D = fft.transform(complex1D)
for i in range(4):
    ott.assert_almost_equal(transform1D[i].real, reference1D[i].real, 1e-14, 0.0)
    ott.assert_almost_equal(transform1D[i].imag, reference1D[i].imag, 1e-14, 0.0)
print("single dimension OK")

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

transformedSample = fft.transform(sampleFlat, [rows, cols])
print("2D FFT size = ", transformedSample.getSize())

inverseSample = fft.inverseTransform(transformedSample, [rows, cols])
print("2D FFT back size = ", inverseSample.getSize())

for i in range(rows * cols):
    ott.assert_almost_equal(inverseSample[i].real, sampleFlat[i].real, 1e-12, 1e-12)
    ott.assert_almost_equal(inverseSample[i].imag, sampleFlat[i].imag, 0.0, 1e-12)
print("2D roundtrip OK")

# 3D case
ot.RandomGenerator.SetSeed(0)
elements = [ot.RandomGenerator.Generate() for i in range(N * N * N)]
tensorFlat = ot.ComplexCollection(
    [complex(elements[i + j * N + k * N * N], 0.0)
     for k in range(N) for j in range(N) for i in range(N)]
)

transformedTensor = fft.transform(tensorFlat, [N, N, N])
print("3D FFT size = ", transformedTensor.getSize())

inverseTensor = fft.inverseTransform(transformedTensor, [N, N, N])
print("3D FFT back size = ", inverseTensor.getSize())

for i in range(N * N * N):
    ott.assert_almost_equal(inverseTensor[i].real, tensorFlat[i].real, 1e-12, 1e-12)
    ott.assert_almost_equal(inverseTensor[i].imag, tensorFlat[i].imag, 0.0, 1e-12)
print("3D roundtrip OK")

# Multidimensional forward comparison with KissFFT: the roundtrips alone
# would not catch a layout or sign error consistent between the transform
# and the inverseTransform, this checks the column-major axis order against
# the reference back-end
kiss = ot.KissFFT()
complex3D = ot.ComplexCollection(
    [complex(elements[i], elements[(i + 7) % 512]) for i in range(30)]
)
kissTransform3D = ot.ComplexCollection(kiss.transform(complex3D, [2, 3, 5]))
pocketTransform3D = ot.ComplexCollection(fft.transform(complex3D, [2, 3, 5]))
for i in range(30):
    ott.assert_almost_equal(pocketTransform3D[i].real, kissTransform3D[i].real, 1e-12, 1e-12)
    ott.assert_almost_equal(pocketTransform3D[i].imag, kissTransform3D[i].imag, 1e-12, 1e-12)
print("3D forward comparison with KissFFT OK")

# 4D transform + roundtrip
vals = [i + 1.0 for i in range(120)]
data = ot.Point(vals)
transform4D = fft.transform(data, [2, 3, 4, 5])
inverse4D = fft.inverseTransform(transform4D, [2, 3, 4, 5])
ott.assert_almost_equal(
    max(abs(inverse4D[i] - data[i]) for i in range(len(data))), 0.0, 0.0, 1e-10
)
print("4D roundtrip OK")

# Rectangular shapes, including an axis of size 1 and a prime axis
for shape in [[2, 3, 5], [1, 7], [4, 1, 3], [3, 13]]:
    length = 1
    for axis in shape:
        length *= axis
    flat = ot.Point([float(i + 1) for i in range(length)])
    transformFlat = fft.transform(flat, shape)
    restoredFlat = fft.inverseTransform(transformFlat, shape)
    for i in range(length):
        ott.assert_almost_equal(restoredFlat[i].real, flat[i], 1e-12, 1e-12)
    print("shape ", shape, "roundtrip OK")

# The normalization of the inverse ND transform is applied on the last axis,
# so that subnormal values are summed before being scaled instead of being
# flushed to zero by the scaling of the first axis
minSubnormal = float.fromhex("0x1p-1022")
subnormalData = ot.ComplexCollection(
    [complex(minSubnormal, 0.0), complex(minSubnormal, 0.0)]
)
subnormalInverse = ot.ComplexCollection(fft.inverseTransform(subnormalData, [1, 2]))
ott.assert_almost_equal(subnormalInverse[0].real, minSubnormal, 0.0, 0.0)
ott.assert_almost_equal(subnormalInverse[1].real, 0.0, 0.0, 0.0)
print("subnormal inverse normalization OK")

# Errors on bad size and zero dimension
with ott.assert_raises(TypeError):
    fft.transform(ot.ComplexCollection([1.0, 2.0, 3.0]), [2, 3])
with ott.assert_raises(TypeError):
    fft.transform(ot.ComplexCollection(0), [0])
with ott.assert_raises(TypeError):
    fft.transform(ot.ComplexCollection(0), [2, 0])
print("size and dimension errors OK")

# The algorithm can be used by a process expecting a FFT algorithm
ot.RandomGenerator.SetSeed(42)
covarianceModel = ot.SquaredExponential([1.0], [1.0])
interval = ot.Interval(0.0, 10.0)
process = otexp.CirculantEmbeddingGaussianProcess(covarianceModel, interval, [31])
process.setFFTAlgorithm(otexp.PocketFFT())
print("process FFT algorithm = ", process.getFFTAlgorithm())
realization = process.getRealization()
print("realization size = ", realization.getValues().getSize())

# The algorithm is saved and loaded under its own name by a study
fname = "pocketfft_study.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(fname))
study.add("fft", otexp.PocketFFT())
study.save()
with open(fname) as file:
    assert 'class="PocketFFT"' in file.read(), "wrong class name in the saved study"
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(fname))
loadedStudy.load()
assert loadedStudy.hasObject("fft")
loadedAlgorithm = ot.FFT()
loadedStudy.fillObject("fft", loadedAlgorithm)
restoredAlgorithm = loadedAlgorithm.getImplementation()
print("restored class = ", restoredAlgorithm.__class__.__name__)
assert restoredAlgorithm.__class__.__name__ == "PocketFFT"
# The restored algorithm still transforms data
restoredTransform = ot.ComplexCollection(restoredAlgorithm.transform(complex2D, [2, 3]))
for i in range(6):
    ott.assert_almost_equal(restoredTransform[i].real, transform2DComplex[i].real, 1e-14, 0.0)
print("study roundtrip OK")

os.remove(fname)
