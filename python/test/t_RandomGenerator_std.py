#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Test the initialization method
ot.RandomGenerator.SetSeed(0)

# Test the state accessor for reading
initialState = ot.RandomGenerator.GetState()
print("initialState=", repr(initialState))
# Test the generation of one realization
realization = ot.RandomGenerator.Generate()
print("one realization=%.6f" % realization)
state = ot.RandomGenerator.GetState()
print("state=", repr(state))
print("new state index=", state.getIndex())

# Test the generation of several realizations
size = 10000
several = ot.RandomGenerator.Generate(size)
print(size, " realizations, first=%.6f" % several[0], ", last=%.6f" % several[size - 1])

# Test the state accessor for writing
ot.RandomGenerator.SetState(initialState)
print("restoring initial state, one realization=%.6f" % ot.RandomGenerator.Generate())
# Test the uniformity of the random generator
size = 100000
manyPoints = ot.RandomGenerator.Generate(size)
slice = 10
frequencies = ot.Point(slice, 0.0)
mean = 0.0
for i in range(size):
    frequencies[int(slice * manyPoints[i])] = (
        frequencies[int(slice * manyPoints[i])] + 1
    )
    mean = mean + manyPoints[i]

mean = mean / size
print("mean of", size, "realizations= %.12f" % mean)
for i in range(slice):
    frequencies[i] = frequencies[i] / size
    print(
        "frequency in slice [",
        (1.0 * i) / slice,
        ",",
        (i + 1.0) / slice,
        "]=",
        frequencies[i],
    )

print(
    "One integer generation in [[0, ",
    slice - 1,
    "]]=",
    ot.RandomGenerator.IntegerGenerate(slice),
)
print(
    "Another integer generation in [[0, ",
    slice - 1,
    "]]=",
    ot.RandomGenerator.IntegerGenerate(slice),
)
manyUnsignedInteger = ot.RandomGenerator.IntegerGenerate(size, slice)
frequencies = ot.Point(slice, 0.0)
mean = 0.0
for i in range(size):
    frequencies[manyUnsignedInteger[i]] = frequencies[manyUnsignedInteger[i]] + 1
    mean = mean + manyUnsignedInteger[i]

mean = mean / size
print("mean of ", size, " integer realizations=", mean)
for i in range(slice):
    frequencies[i] = frequencies[i] / size
    print("frequency for value ", i, "=", frequencies[i])
