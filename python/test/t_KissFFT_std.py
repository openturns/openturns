#! /usr/bin/env python

import cmath

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# ---------------------------------------------------------------------------
# FFT interface tests via KissFFT
# ---------------------------------------------------------------------------

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

# Test __repr__ and __str__
kissFFT = ot.KissFFT()
print("kissFFT = ", kissFFT)
str_repr = repr(kissFFT)
str_val = str(kissFFT)
ott.assert_almost_equal("KissFFT" in str_repr, True, 0.0, 0.0)
ott.assert_almost_equal("KissFFT" in str_val, True, 0.0, 0.0)

# Initial transformation
print("collection = ", collection)

# FFT transform
transformedCollection = ot.ComplexCollection(kissFFT.transform(collection))
print("FFT result = ", transformedCollection)

# Inverse transformation
inverseTransformedCollection = ot.ComplexCollection(kissFFT.inverseTransform(transformedCollection))
print("FFT back=", inverseTransformedCollection)

# Check 1D complex roundtrip
for i in range(size):
    ott.assert_almost_equal(inverseTransformedCollection[i], collection[i], 1e-13, 1e-13)

# ---------------------------------------------------------------------------
# FFT on real (Scalar) collection via the FFTImplementation base class wrapper
# ---------------------------------------------------------------------------
print("\nTest FFT on real scalar collection")
fft = ot.FFT()
# FFTImplementation base class converts real -> complex internally
realCollection = ot.ScalarCollection([1.0, 2.0, 3.0, 4.0])
resultReal = fft.transform(realCollection)
ott.assert_almost_equal(len(resultReal), 4, 0.0, 0.0)
# The result should be complex
for i in range(4):
    ott.assert_almost_equal(resultReal[i].imag, 0.0, 1e-14, 0.0)

# Inverse of real FFT
invResultReal = fft.inverseTransform(resultReal)
for i in range(4):
    ott.assert_almost_equal(invResultReal[i].real, realCollection[i], 1e-13, 0.0)

print("Real scalar FFT roundtrip OK")

# ---------------------------------------------------------------------------
# FFT transform with first/size parameters (partial transform)
# ---------------------------------------------------------------------------
print("\nTest FFT transform with first/size")
bigCollection = ot.ComplexCollection(32)
for i in range(32):
    bigCollection[i] = complex(0.1 * (i + 1) / 32, 0.3 * (i + 1) / 32)

# Transform a subset: elements [4, 8)
partialResult = fft.transform(bigCollection, 4, 4)
ott.assert_almost_equal(len(partialResult), 4, 0.0, 0.0)

# Verify the partial transform matches a full transform of the subset
subset = ot.ComplexCollection(4)
for i in range(4):
    subset[i] = bigCollection[4 + i]
fullResult = fft.transform(subset)
for i in range(4):
    ott.assert_almost_equal(partialResult[i], fullResult[i], 1e-14, 1e-14)

print("Partial transform OK")

# Test inverseTransform with first/size
partialInv = fft.inverseTransform(transformedCollection, 0, size)
for i in range(size):
    ott.assert_almost_equal(partialInv[i], collection[i], 1e-13, 1e-13)

print("Inverse partial transform OK")

# ---------------------------------------------------------------------------
# FFT transform with first/step/last (NotYetImplementedException)
# ---------------------------------------------------------------------------
print("\nTest FFT transform with first/step/last")
with ott.assert_raises(Exception):
    fft.transform(collection, 0, 1, 8)
print("first/step/last exception OK")

# Inverse transform with first/step/last (NotYetImplementedException)
with ott.assert_raises(Exception):
    fft.inverseTransform(collection, 0, 1, 8)
print("Inverse first/step/last exception OK")

# Inverse transform of real with first/step/last (NotYetImplementedException)
with ott.assert_raises(Exception):
    fft.inverseTransform(realCollection, 0, 1, 4)
print("Inverse real first/step/last exception OK")

# Inverse transform of complex with first/size
partialComplexInv = fft.inverseTransform(transformedCollection, 0, size)
for i in range(size):
    ott.assert_almost_equal(partialComplexInv[i], collection[i], 1e-13, 1e-13)
print("Inverse partial complex transform OK")

# ---------------------------------------------------------------------------
# 2D case via ND
# ---------------------------------------------------------------------------
print("\nTest 2D FFT via ND")
N = 8
distribution = ot.Normal(N)
sample = distribution.getSample(2 * N)

rows = sample.getSize()
cols = sample.getDimension()
sampleFlat = ot.ComplexCollection([complex(sample[i, j], 0.0) for i in range(rows) for j in range(cols)])
transformedSample = kissFFT.transform(sampleFlat, [rows, cols])
print("2D FFT result = ", repr(transformedSample))

# Inverse transformation
inverseTransformedSample = kissFFT.inverseTransform(transformedSample, [rows, cols])
print("2D FFT back=", repr(inverseTransformedSample))

# Check 2D roundtrip
for i in range(rows * cols):
    ott.assert_almost_equal(inverseTransformedSample[i], sampleFlat[i], 1e-14, 1e-14)

# ---------------------------------------------------------------------------
# 3D case via ND
# ---------------------------------------------------------------------------
print("\nTest 3D FFT via ND")
elements = [ot.RandomGenerator.Generate() for i in range(N * N * N)]
tensorFlat = ot.ComplexCollection([complex(elements[i + j * N + k * N * N], 0.0) for k in range(N) for j in range(N) for i in range(N)])

transformedTensor = kissFFT.transform(tensorFlat, [N, N, N])
print("3D FFT result = ", repr(transformedTensor))

inverseTransformedTensor = kissFFT.inverseTransform(transformedTensor, [N, N, N])
print("3D FFT back=", repr(inverseTransformedTensor))

# Check 3D roundtrip
for i in range(N * N * N):
    ott.assert_almost_equal(inverseTransformedTensor[i], tensorFlat[i], 1e-14, 1e-14)

# ---------------------------------------------------------------------------
# 2D ND transform on real Point data
# ---------------------------------------------------------------------------
print("\nTest 2D ND FFT on real Point data")
realData = ot.Point([1.0, 2.0, 3.0, 4.0, 5.0, 6.0])
result2DReal = fft.transform(realData, [2, 3])
ott.assert_almost_equal(len(result2DReal), 6, 0.0, 0.0)

# Roundtrip
inv2DReal = fft.inverseTransform(result2DReal, [2, 3])
for i in range(6):
    ott.assert_almost_equal(inv2DReal[i], realData[i], 1e-14, 0.0)

print("2D real ND roundtrip OK")

# ---------------------------------------------------------------------------
# 2D ND transform on complex data
# ---------------------------------------------------------------------------
print("\nTest 2D ND FFT on complex data")
complexData = ot.ComplexCollection([complex(1.0, 0.0), complex(2.0, 0.5), complex(3.0, 0.0), complex(4.0, 0.5), complex(5.0, 0.0), complex(6.0, 0.5)])
result2DCmplx = fft.transform(complexData, [2, 3])
ott.assert_almost_equal(len(result2DCmplx), 6, 0.0, 0.0)

inv2DCmplx = fft.inverseTransform(result2DCmplx, [2, 3])
for i in range(6):
    ott.assert_almost_equal(inv2DCmplx[i], complexData[i], 1e-14, 1e-14)

print("2D complex ND roundtrip OK")

# ---------------------------------------------------------------------------
# 3D ND transform on real Point data
# ---------------------------------------------------------------------------
print("\nTest 3D ND FFT on real Point data")
realData3D = ot.Point([i + 1.0 for i in range(24)])
result3DReal = fft.transform(realData3D, [2, 3, 4])
ott.assert_almost_equal(len(result3DReal), 24, 0.0, 0.0)

inv3DReal = fft.inverseTransform(result3DReal, [2, 3, 4])
for i in range(24):
    ott.assert_almost_equal(inv3DReal[i], realData3D[i], 1e-14, 0.0)

print("3D real ND roundtrip OK")

# ---------------------------------------------------------------------------
# 3D ND transform on complex data
# ---------------------------------------------------------------------------
print("\nTest 3D ND FFT on complex data")
complexData3D = ot.ComplexCollection([complex(i + 1.0, 0.1 * (i + 1)) for i in range(24)])
result3DCmplx = fft.transform(complexData3D, [2, 3, 4])
ott.assert_almost_equal(len(result3DCmplx), 24, 0.0, 0.0)

inv3DCmplx = fft.inverseTransform(result3DCmplx, [2, 3, 4])
for i in range(24):
    ott.assert_almost_equal(inv3DCmplx[i], complexData3D[i], 1e-14, 1e-14)

print("3D complex ND roundtrip OK")

# ---------------------------------------------------------------------------
# 4D ND transform + roundtrip
# ---------------------------------------------------------------------------
print("\nTest 4D ND FFT roundtrip")
vals4D = [i + 1.0 for i in range(120)]
data4D = ot.Point(vals4D)
r4D = fft.transform(data4D, [2, 3, 4, 5])
r4D_inv = fft.inverseTransform(r4D, [2, 3, 4, 5])
for i in range(len(data4D)):
    ott.assert_almost_equal(r4D_inv[i], data4D[i], 1e-14, 0.0)

print("4D ND roundtrip OK")

# ---------------------------------------------------------------------------
# ND error: mismatched dimensions
# ---------------------------------------------------------------------------
print("\nTest ND FFT dimension mismatch")
with ott.assert_raises(Exception):
    fft.transform(data4D, [2, 3, 5])
print("Dimension mismatch error OK")

# ---------------------------------------------------------------------------
# Verify Parseval-like energy conservation for complex 1D
# ---------------------------------------------------------------------------
print("\nTest energy conservation (Parseval)")
N_parseval = 32
signal = ot.ComplexCollection(N_parseval)
for i in range(N_parseval):
    signal[i] = complex(0.5 * (i + 1) / N_parseval, -0.2 * (i + 1) / N_parseval)
spectrum = kissFFT.transform(signal)
energyTime = sum(abs(signal[i]) ** 2 for i in range(N_parseval))
energyFreq = sum(abs(spectrum[i]) ** 2 for i in range(N_parseval)) / N_parseval
ott.assert_almost_equal(energyTime, energyFreq, 1e-10, 0.0)
print("Parseval energy conservation OK")

# ---------------------------------------------------------------------------
# Verify linearity: FFT(a*x + b*y) = a*FFT(x) + b*FFT(y)
# ---------------------------------------------------------------------------
print("\nTest FFT linearity")
N_lin = 16
x = ot.ComplexCollection(N_lin)
y = ot.ComplexCollection(N_lin)
for i in range(N_lin):
    x[i] = complex(0.1 * (i + 1) / N_lin, 0.0)
    y[i] = complex(0.0, 0.2 * (i + 1) / N_lin)
a, b = 2.0 + 0.5j, 1.0 - 1.0j
combined = ot.ComplexCollection(N_lin)
for i in range(N_lin):
    combined[i] = a * x[i] + b * y[i]
fftCombined = kissFFT.transform(combined)
fftX = kissFFT.transform(x)
fftY = kissFFT.transform(y)
for i in range(N_lin):
    expected = a * fftX[i] + b * fftY[i]
    ott.assert_almost_equal(fftCombined[i], expected, 1e-10, 1e-10)
print("FFT linearity OK")

# ---------------------------------------------------------------------------
# Verify shift theorem: FFT shifted signal has known phase relationship
# ---------------------------------------------------------------------------
print("\nTest FFT shift theorem")
N_shift = 8
base = ot.ComplexCollection(N_shift)
for i in range(N_shift):
    base[i] = complex(1.0 if i < N_shift // 2 else 0.0, 0.0)
shifted = ot.ComplexCollection(N_shift)
shiftAmount = 2
for i in range(N_shift):
    shifted[i] = base[(i - shiftAmount) % N_shift]
fftBase = kissFFT.transform(base)
fftShifted = kissFFT.transform(shifted)
for k in range(N_shift):
    phase = cmath.exp(-2j * cmath.pi * k * shiftAmount / N_shift)
    expected = fftBase[k] * phase
    ott.assert_almost_equal(fftShifted[k], expected, 1e-10, 1e-10)
print("FFT shift theorem OK")

print("\nAll FFT/FFTImplementation tests passed!")
