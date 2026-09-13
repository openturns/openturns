#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

fft = ot.KissFFT()

# 1D transform
data = ot.ComplexCollection([1.0, 2.0, 3.0, 4.0])
r1 = fft.transform(data, [4])
r2 = fft.transform(data)
assert max(abs(r1[i] - r2[i]) for i in range(4)) < 1e-14, "1D mismatch"
print("1D ND OK")

# 2D transform + roundtrip
vals = [1.0, 4.0, 2.0, 5.0, 3.0, 6.0]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3])
r2 = fft.inverseTransform(r1, [2, 3])
for i in range(6):
    ott.assert_almost_equal(r2[i], data[i], 1e-14, 0.0)
print("2D ND OK")

# 3D transform
vals = [i + 1.0 for i in range(24)]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3, 4])
print("3D ND OK")

# Inverse roundtrip
r3 = fft.inverseTransform(r1, [2, 3, 4])
diff = max(abs(r3[i] - data[i]) for i in range(len(data)))
assert diff < 1e-14, "3D inverse roundtrip mismatch"
print("3D inverse roundtrip OK")

# Complex collection input (should match Point input)
data2 = ot.ComplexCollection(vals)
r4 = fft.transform(data2, [2, 3, 4])
diff = max(abs(r4[i] - r1[i]) for i in range(len(vals)))
assert diff < 1e-14, "Complex input mismatch"
print("Complex input OK")

# Check error on bad size
with ott.assert_raises(TypeError):
    fft.transform(data, [2, 3, 5])
print("Bad size error OK")

# Check error on zero dimension (empty data passes size check but would divide by zero)
empty = ot.ComplexCollection(0)
with ott.assert_raises(TypeError):
    fft.transform(empty, [0])
with ott.assert_raises(TypeError):
    fft.transform(empty, [2, 0])
print("zero dimension rejected OK")

# 4D transform + roundtrip
vals = [i + 1.0 for i in range(120)]
data = ot.Point(vals)
r1 = fft.transform(data, [2, 3, 4, 5])
r2 = fft.inverseTransform(r1, [2, 3, 4, 5])
diff = max(abs(r2[i] - data[i]) for i in range(len(data)))
assert diff < 1e-14, "4D inverse roundtrip mismatch"
print("4D roundtrip OK")
