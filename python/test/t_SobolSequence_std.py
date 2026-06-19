#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import math as m

ot.TESTPREAMBLE()


# Create 6 points from a Sobol' sequence in dimension 1
expected = ot.Sample(
    [
        [1.0 / 2.0],
        [3.0 / 4.0],
        [1.0 / 4.0],
        [3.0 / 8.0],
        [7.0 / 8.0],
        [5.0 / 8.0],
    ]
)
sequence = ot.SobolSequence(1)
print(sequence)
sobolSample = sequence.generate(6)
ott.assert_almost_equal(sobolSample, expected)

# Create 6 points from a Sobol' sequence in dimension 2
expected = ot.Sample(
    [
        [1.0 / 2.0, 1.0 / 2.0],
        [3.0 / 4.0, 1.0 / 4.0],
        [1.0 / 4.0, 3.0 / 4.0],
        [3.0 / 8.0, 3.0 / 8.0],
        [7.0 / 8.0, 7.0 / 8.0],
        [5.0 / 8.0, 1.0 / 8.0],
    ]
)
sequence = ot.SobolSequence(2)
print(sequence)
sobolSample = sequence.generate(6)
ott.assert_almost_equal(sobolSample, expected)

# Create a Sobol' sequence of maximum dimension
sequence = ot.SobolSequence(ot.SobolSequence.MaximumDimension)
print(sequence)
sobolSample = sequence.generate(10)

# Create a Sobol' sequence of dimension 2 to estimate Pi in [0 1)^2
dimension = 2
sequence = ot.SobolSequence(dimension)
pointInsideCircle = 0
sampleSize = 2**11  # This is significant!
for i in range(sampleSize):
    sobolPoint = sequence.generate()
    if sobolPoint.norm() < 1.0:
        pointInsideCircle += 1
probabilityEstimate = (1.0 * pointInsideCircle) / sampleSize
probability = m.pi / 4.0
print("sample size=", sampleSize)
print("computed probability =", probabilityEstimate)
print("expected probability =", probability)
rtol = 10.0 / sampleSize
ott.assert_almost_equal(probability, probabilityEstimate, rtol)

# Test scrambling modes

# MULTIDIGIT scrambling
sequence = ot.SobolSequence(2, "MULTIDIGIT")
print(sequence)
sample = sequence.generate(5)
print(sample)

# Invalid scrambling
with ott.assert_raises(TypeError):
    ot.SobolSequence(2, "INVALID")

# Test setScrambling
sequence = ot.SobolSequence(2)
print(sequence)
assert sequence.getScrambling() == "NONE"
# After changing scrambling, the sequence should be reinitialized
sequence.setScrambling("MULTIDIGIT")
assert sequence.getScrambling() == "MULTIDIGIT"

# Test exceptions due to bad ResourceMap values
old_bits = ot.ResourceMap.GetAsUnsignedInteger("SobolSequence-MultidigitBits")
old_multiplier = ot.ResourceMap.GetAsUnsignedInteger(
    "SobolSequence-MultidigitMultiplier"
)
try:
    # Invalid MultidigitBits (too small)
    ot.ResourceMap.SetAsUnsignedInteger("SobolSequence-MultidigitBits", 0)
    with ott.assert_raises(TypeError):
        ot.SobolSequence(2, "MULTIDIGIT")

    # Invalid MultidigitBits (too large)
    ot.ResourceMap.SetAsUnsignedInteger("SobolSequence-MultidigitBits", 100)
    with ott.assert_raises(TypeError):
        ot.SobolSequence(2, "MULTIDIGIT")

    # Invalid MultidigitMultiplier (zero)
    ot.ResourceMap.SetAsUnsignedInteger("SobolSequence-MultidigitMultiplier", 0)
    with ott.assert_raises(TypeError):
        ot.SobolSequence(2, "MULTIDIGIT")
finally:
    ot.ResourceMap.SetAsUnsignedInteger("SobolSequence-MultidigitBits", old_bits)
    ot.ResourceMap.SetAsUnsignedInteger(
        "SobolSequence-MultidigitMultiplier", old_multiplier
    )
