#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

fft = ot.KissFFT()

# 1D transform
data = ot.ComplexCollection([1.0, 2.0, 3.0, 4.0])
r1 = fft.transform(data, [4])
r2 = fft.transform(data)
for i in range(4):
    ott.assert_almost_equal(r1[i], r2[i], 1e-14, 0.0)
print("1D ND OK")

# 2D transform
vals = [1.0, 4.0, 2.0, 5.0, 3.0, 6.0]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3])
print("2D ND OK")

# 3D transform
vals = [i + 1.0 for i in range(24)]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3, 4])
print("3D ND OK")

# Inverse roundtrip
r3 = fft.inverseTransform(r1, [2, 3, 4])
for i in range(len(data)):
    ott.assert_almost_equal(r3[i], data[i], 1e-14, 0.0)
print("3D inverse roundtrip OK")

# Complex collection input (should match Point input)
data2 = ot.ComplexCollection(vals)
r4 = fft.transform(data2, [2, 3, 4])
for i in range(len(vals)):
    ott.assert_almost_equal(r4[i], r1[i], 1e-14, 0.0)
print("Complex input OK")

# Check error on bad size
with ott.assert_raises(Exception):
    fft.transform(data, [2, 3, 5])
print("Bad size error OK")

# 4D transform + roundtrip
vals = [i + 1.0 for i in range(120)]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3, 4, 5])
r2 = fft.inverseTransform(r1, [2, 3, 4, 5])
for i in range(len(data)):
    ott.assert_almost_equal(r2[i], data[i], 1e-14, 0.0)
print("4D roundtrip OK")

# ---------------------------------------------------------------------------
# Additional ND tests via FFT wrapper
# ---------------------------------------------------------------------------
print("\nTest additional ND tests via FFT wrapper")
fftWrapper = ot.FFT()

# 1D real Point transform + roundtrip
realData1D = ot.Point([1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0])
spec1D = fftWrapper.transform(realData1D, [8])
ott.assert_almost_equal(len(spec1D), 8, 0.0, 0.0)
inv1D = fftWrapper.inverseTransform(spec1D, [8])
for i in range(8):
    ott.assert_almost_equal(inv1D[i], realData1D[i], 1e-14, 0.0)
print("1D real ND roundtrip OK")

# 2D real Point transform + roundtrip
realData2D = ot.Point([1.0, 2.0, 3.0, 4.0, 5.0, 6.0])
spec2D = fftWrapper.transform(realData2D, [2, 3])
inv2D = fftWrapper.inverseTransform(spec2D, [2, 3])
for i in range(6):
    ott.assert_almost_equal(inv2D[i], realData2D[i], 1e-14, 0.0)
print("2D real ND roundtrip OK")

# 3D real Point transform + roundtrip
realData3D = ot.Point([i + 1.0 for i in range(60)])
spec3D = fftWrapper.transform(realData3D, [3, 4, 5])
inv3D = fftWrapper.inverseTransform(spec3D, [3, 4, 5])
for i in range(60):
    ott.assert_almost_equal(inv3D[i], realData3D[i], 1e-14, 0.0)
print("3D real ND roundtrip OK")

# 2D complex Point transform + roundtrip
complexData2D = ot.ComplexCollection([complex(i + 1, 0.1 * (i + 1)) for i in range(12)])
specCmplx2D = fftWrapper.transform(complexData2D, [3, 4])
invCmplx2D = fftWrapper.inverseTransform(specCmplx2D, [3, 4])
for i in range(12):
    ott.assert_almost_equal(invCmplx2D[i], complexData2D[i], 1e-14, 1e-14)
print("2D complex ND roundtrip OK")

# 3D complex Point transform + roundtrip
complexData3D = ot.ComplexCollection([complex(i + 1, 0.05 * (i + 1)) for i in range(48)])
specCmplx3D = fftWrapper.transform(complexData3D, [4, 3, 4])
invCmplx3D = fftWrapper.inverseTransform(specCmplx3D, [4, 3, 4])
for i in range(48):
    ott.assert_almost_equal(invCmplx3D[i], complexData3D[i], 1e-14, 1e-14)
print("3D complex ND roundtrip OK")

# 5D transform + roundtrip
vals5D = [i + 1.0 for i in range(120)]
data5D = ot.Point(vals5D)
spec5D = fftWrapper.transform(data5D, [2, 3, 4, 5, 1])
inv5D = fftWrapper.inverseTransform(spec5D, [2, 3, 4, 5, 1])
for i in range(120):
    ott.assert_almost_equal(inv5D[i], data5D[i], 1e-14, 0.0)
print("5D ND roundtrip OK")

# ---------------------------------------------------------------------------
# Verify energy conservation (Parseval) in 2D
# ---------------------------------------------------------------------------
print("\nTest 2D energy conservation")
N1, N2 = 4, 6
signal2D = ot.ComplexCollection(N1 * N2)
for i in range(N1 * N2):
    signal2D[i] = complex(0.1 * (i + 1) / (N1 * N2), -0.05 * (i + 1) / (N1 * N2))
spectrum2D = fftWrapper.transform(signal2D, [N1, N2])
energyTime = sum(abs(signal2D[i]) ** 2 for i in range(N1 * N2))
energyFreq = sum(abs(spectrum2D[i]) ** 2 for i in range(N1 * N2)) / (N1 * N2)
ott.assert_almost_equal(energyTime, energyFreq, 1e-10, 0.0)
print("2D Parseval OK")

# ---------------------------------------------------------------------------
# Verify energy conservation (Parseval) in 3D
# ---------------------------------------------------------------------------
print("\nTest 3D energy conservation")
N1, N2, N3 = 3, 4, 5
signal3D = ot.ComplexCollection(N1 * N2 * N3)
for i in range(N1 * N2 * N3):
    signal3D[i] = complex(0.2 * (i + 1) / (N1 * N2 * N3), 0.1 * (i + 1) / (N1 * N2 * N3))
spectrum3D = fftWrapper.transform(signal3D, [N1, N2, N3])
energyTime = sum(abs(signal3D[i]) ** 2 for i in range(N1 * N2 * N3))
energyFreq = sum(abs(spectrum3D[i]) ** 2 for i in range(N1 * N2 * N3)) / (N1 * N2 * N3)
ott.assert_almost_equal(energyTime, energyFreq, 1e-10, 0.0)
print("3D Parseval OK")

print("\nAll KissFFT ND tests passed!")
