#! /usr/bin/env python

import contextlib
import io

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# We create an empty Sample
sample1 = ot.Sample(0, 2)
sample1.setName("Sample1")
sample1.setDescription(("a0", "a1"))

# We populate the empty sample
point1 = ot.Point(2)
point1[0] = 10.0
point1[1] = 20.0
point2 = ot.Point(2)
point2[0] = 11.0
point2[1] = 21.0
point3 = ot.Point(2)
point3[0] = 12.0
point3[1] = 22.0
sample1.add(point1)
sample1.add(point2)
sample1.add(point3)
print("sample1=", repr(sample1))

# We get the second element of the sample
secondElement = sample1[1]
print("second element=", repr(secondElement))

# We set the third element to a valid new element
newPoint = ot.Point(2)
newPoint[0] = 1000
newPoint[1] = 2000
sample1[2] = newPoint
print("sample1=", repr(sample1))

sample2 = ot.Sample(10, point1)
print("sample2=", repr(sample2))

sample2[5] = point2
print("sample2=", repr(sample2))

with ott.assert_raises(IndexError):
    # We get the tenth element of the sample
    # THIS SHOULD NORMALLY FAIL
    tenthElement = sample1[9]

# We try to create a sample with 5 times the same point
samePoint = ot.Point(3)
samePoint[0] = 1000.0
samePoint[1] = 2000.0
samePoint[2] = 3000.0
sample3 = ot.Sample(5, samePoint)
print("sample3=", repr(sample3))

# We append a new point to the sample...
anotherNewPoint = ot.Point(3)
anotherNewPoint[0] = -1000.0
anotherNewPoint[1] = -2000.0
anotherNewPoint[2] = -3000.0
sample3.add(anotherNewPoint)
print("sample3=", repr(sample3))

# ... then the first point again
sample3.add(samePoint)
print("sample3=", repr(sample3))

# We find the new point into the sample
pos = sample3.index(anotherNewPoint)
if pos != 5:
    print("Error in finding point into sample. Expected 5. Got ", pos)

# We erase the new point from the sample
sample3.erase(pos)
print("sample3=", repr(sample3))

# Partial copy constructor containing sample2[4:7]
sample4 = ot.Sample(sample2, 4, 8)
print("sample4=", repr(sample4))

sample5 = ot.Sample([[1.0], [3.0], [2.0]])
print("sample5=", repr(sample5))
print("sample5 (linear)=", sample5.asPoint())

# Selection
indices = [1, 0, 2, 1, 2]
print("selection=", sample5.select(indices))
indices = list(range(len(sample5)))
print("selection(id)=", sample5.select(indices))

# html conversion
print(sample1._repr_html_())

# check conversion
v1 = [ot.Point([12.0]), ot.Point([1.0])]
v2 = [ot.Point([13.0]), ot.Point([2.0])]
with ott.assert_raises(TypeError):
    s = ot.Sample([v1, v2])

with ott.assert_raises(TypeError):
    # uneven points
    s = ot.Sample([[1.0, 2.0], [5.0]])

# getMarginal by identifiers
sample = ot.Normal(4).getSample(5)
assert sample.getMarginal(["X3", "X1"]).getDescription() == [
    "X3",
    "X1",
], "wrong marginal"

# argsort
sample = ot.Sample(
    [[-1.0, 1.0, 0.0], [-1.0, 1.0, 1.0], [-1.0, 0.0, 1.0], [-1.0, 0.0, -1.0]]
)
indices = sample.argsort()
print("indices = ", indices)
assert indices == [3, 2, 0, 1]
indices = sample.argsort(False)
print("indices = ", indices)
assert indices == [1, 0, 2, 3]


# this should not crash


def centeredSumOfProduct(sampleA, sampleB, center):
    sampleA -= center
    sampleB -= center
    return sampleA.asPoint().dot(sampleB.asPoint())


YA = ot.Sample(10, 1)
center = 0.0
sumOfSquaresOfG = centeredSumOfProduct(YA, YA, center)

# html conversion of a Sample with fractional numbers
sampleFractional = ot.Sample(
    [
        [0.1234567890, 0.3333333333],
        [-123456.78901, 0.6666666666],
        [1.2345678901e10, -1.2345678901e-12],
    ]
)
print(sampleFractional._repr_html_())

# erase a set of indices, see issue #1730
sample = ot.Sample([[float(x)] for x in range(10)])
sample.erase([3, 5, 8])
ott.assert_almost_equal(sample, ot.Sample([[0.0], [1.0], [2.0], [4.0], [6.0], [7.0], [9.0]]))
# unsorted indices are fine
sample = ot.Sample([[float(x)] for x in range(10)])
sample.erase([8, 3, 5])
ott.assert_almost_equal(sample, ot.Sample([[0.0], [1.0], [2.0], [4.0], [6.0], [7.0], [9.0]]))
# empty list is a no-op
reference = ot.Sample([[float(x)] for x in range(10)])
sample = ot.Sample(reference)
sample.erase([])
ott.assert_almost_equal(sample, reference)
# invalid calls raise
for bad in ([10], [-1], [2, 2]):
    sample = ot.Sample([[float(x)] for x in range(10)])
    with ott.assert_raises(Exception):
        sample.erase(bad)

# Descriptive statistics, see issue #3079
descSample = ot.Sample([[1.0, 10.0], [2.0, 20.0], [3.0, 30.0], [4.0, 40.0]])
descSample.setDescription(["x", "y"])
stats = descSample.describe()
assert list(stats["count"]) == [4.0, 4.0]
ott.assert_almost_equal(stats["mean"], descSample.computeMean())
ott.assert_almost_equal(stats["std"], descSample.computeStandardDeviation())
ott.assert_almost_equal(stats["min"], descSample.getMin())
ott.assert_almost_equal(stats["max"], descSample.getMax())
ott.assert_almost_equal(
    stats["25%"], descSample.computeQuantilePerComponent(0.25)
)
ott.assert_almost_equal(
    stats["50%"], descSample.computeQuantilePerComponent(0.5)
)
ott.assert_almost_equal(
    stats["75%"], descSample.computeQuantilePerComponent(0.75)
)
custom = descSample.describe([0.1, 0.9])
ott.assert_almost_equal(
    custom["10%"], descSample.computeQuantilePerComponent(0.1)
)
with ott.assert_raises(ValueError):
    descSample.describe([1.5])
# Closely spaced percentiles keep distinct results
closeStats = descSample.describe([0.12345671, 0.12345672])
percentileKeys = [key for key in closeStats if "%" in key]
assert len(percentileKeys) == 2
assert percentileKeys[0] != percentileKeys[1]
# printSummary writes the same values in a table, capture it for the expout
buffer = io.StringIO()
with contextlib.redirect_stdout(buffer):
    descSample.printSummary()
summary = buffer.getvalue()
assert "mean" in summary and "50%" in summary

# Quantile with tail flag, see issue #3130
tailSample = ot.Normal(1).getSample(50)
for prob in [0.05, 0.5, 0.95]:
    ott.assert_almost_equal(
        tailSample.computeQuantile(prob, True),
        tailSample.computeQuantile(1.0 - prob),
    )
    ott.assert_almost_equal(
        tailSample.computeQuantilePerComponent(prob, True),
        tailSample.computeQuantilePerComponent(1.0 - prob),
    )
# sequence version
probs = [0.05, 0.95]
ott.assert_almost_equal(
    tailSample.computeQuantile(probs, True),
    tailSample.computeQuantile([1.0 - p for p in probs]),
)
# tail improves accuracy close to 1 (no catastrophic 1-p in user code)
ott.assert_almost_equal(
    tailSample.computeQuantile(1.0e-9, True),
    tailSample.computeQuantile(1.0 - 1.0e-9),
)

# Super-quantile (expected shortfall), see issue #3013
superSample = ot.Normal(2).getSample(50)
for prob in [0.0, 0.5, 0.9]:
    threshold = superSample.computeQuantilePerComponent(prob)
    expected = [0.0, 0.0]
    counts = [0, 0]
    for i in range(superSample.getSize()):
        for j in range(2):
            if superSample[i, j] >= threshold[j]:
                expected[j] += superSample[i, j]
                counts[j] += 1
    expected = [expected[j] / counts[j] for j in range(2)]
    ott.assert_almost_equal(superSample.computeSuperQuantile(prob), expected)
with ott.assert_raises(Exception):
    superSample.computeSuperQuantile(1.5)
# Independent oracle: Normal(0, 1) expected shortfall at 0.9 is
# phi(q)/((1 - Phi(q))) with q the 0.9 quantile, about 1.755
ot.RandomGenerator.SetSeed(1)
normalSample = ot.Normal().getSample(20000)
ott.assert_almost_equal(
    normalSample.computeSuperQuantile(0.9), [1.755], 1e-1, 1e-1
)
