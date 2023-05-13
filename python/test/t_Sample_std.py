#! /usr/bin/env python

import openturns as ot

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

try:
    # We get the tenth element of the sample
    # THIS SHOULD NORMALLY FAIL
    tenthElement = sample1[9]

    # Normally, we should never go here
    raise
except Exception:
    pass

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
pos = sample3.find(anotherNewPoint)
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

# html conversion
print(sample1._repr_html_())

# check conversion
v1 = [ot.Point([12.0]), ot.Point([1.0])]
v2 = [ot.Point([13.0]), ot.Point([2.0])]
try:
    s = ot.Sample([v1, v2])
except Exception:
    print("ok")

try:
    # uneven points
    s = ot.Sample([[1.0, 2.0], [5.0]])
except Exception:
    print("ok")

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
sampleFractional = ot.Sample([[0.1234567890, 0.3333333333], [-123456.78901, 0.6666666666], [1.2345678901e10, -1.2345678901e-12]])
print(sampleFractional._repr_html_())
