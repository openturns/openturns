#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
f = ot.SymbolicFunction(["x"], ["2.0 * x"])
antecedent = ot.Normal(0.0, 1.0)
distribution = otexp.PushForwardDistribution(f, antecedent)
print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Is this distribution continuous ?
print("Continuous = ", distribution.isContinuous())

# Dimension of the distribution
print("Dimension = ", distribution.getDimension())
assert distribution.getDimension() == 1

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", oneRealization)
assert oneRealization.getDimension() == 1

# Test for sampling
size = 10000
oneSample = distribution.getSample(size)
print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])

# The distribution of Y = 2X with X ~ Normal(0, 1) is Normal(0, 2)
reference = ot.Normal(0.0, 2.0)

# Compare the PDF against the closed form expression
for point in [-3.0, -1.0, 0.0, 1.0, 3.0]:
    pdf = distribution.computePDF([point])
    print(f"pdf(({point},))={pdf:.12g}")
    ott.assert_almost_equal(pdf, reference.computePDF([point]), 1e-4, 0.0)

# Log PDF consistency with PDF
x = 1.2
logpdf = distribution.computeLogPDF([x])
print(f"log pdf({x})= {logpdf:.12g}")
ott.assert_almost_equal(logpdf, math.log(distribution.computePDF([x])),
                        1e-4, 0.0)

# Range: finite, contains the reference support
rangeValue = distribution.getRange()
print("range=", rangeValue)
assert rangeValue.getDimension() == 1
assert rangeValue.contains([0.0])
assert not rangeValue.contains([50.0])

# Moments estimated by Monte-Carlo
mean = distribution.getMean()
print("mean=", mean)
ott.assert_almost_equal(mean, reference.getMean(), 1e-1, 1e-1)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
ott.assert_almost_equal(standardDeviation,
                        reference.getStandardDeviation(), 1e-1, 0.0)

# Goodness of fit of a sample against the reference distribution
fittingResult = ot.FittingTest.Kolmogorov(oneSample, reference)
print("Kolmogorov p-value= %.12g" % fittingResult.getPValue())
assert fittingResult.getPValue() > 0.05

# Parameters delegation to the antecedent
parameters = distribution.getParametersCollection()
print("parameters=", parameters)
assert parameters.getSize() == 1
assert parameters[0].getDimension() == 2

# Identity function: the distribution is the same as the antecedent
fIdentity = ot.SymbolicFunction(["x"], ["x"])
identityDistribution = otexp.PushForwardDistribution(fIdentity,
                                                     ot.Normal(0.0, 1.0))
ott.assert_almost_equal(identityDistribution.computePDF([0.5]),
                        ot.Normal(0.0, 1.0).computePDF([0.5]))

# Non-injective function: Y = X^2 with X ~ Normal(0, 1).
# The preimage of y > 0 is +/-sqrt(y), so that
# pdf(y) = phi(sqrt(y)) / sqrt(y)
squareFunction = ot.SymbolicFunction(["x"], ["x^2"])
squareDistribution = otexp.PushForwardDistribution(squareFunction,
                                                   ot.Normal(0.0, 1.0))
for y, value in [(1.0, 0.24197072451914337),
                 (0.25, 0.7041306535285990),
                 (4.0, 0.02699548325659403)]:
    pdf = squareDistribution.computePDF([y])
    print(f"square pdf({y})={pdf:.12g}")
    ott.assert_almost_equal(pdf, value, 1e-4, 0.0)
# Points outside the image have a zero density
ott.assert_almost_equal(squareDistribution.computePDF([-1.0]), 0.0, 0.0,
                        1e-12)

# Non-injective function on a finite support: Y = X^2 with
# X ~ Uniform(-1, 1), so that pdf(y) = 1 / (2 sqrt(y))
boundedSquareDistribution = otexp.PushForwardDistribution(
    squareFunction, ot.Uniform(-1.0, 1.0)
)
ott.assert_almost_equal(boundedSquareDistribution.computePDF([0.25]), 1.0,
                        1e-4, 0.0)
ott.assert_almost_equal(boundedSquareDistribution.computePDF([-0.5]), 0.0,
                        0.0, 1e-12)

# Non-injective oscillating function: Y = sin(X) with X ~ Uniform(0, 2 pi).
# The preimages of 0.5 are pi/6 and 5 pi/6, both with |cos| = sqrt(3)/2,
# so that pdf(0.5) = 2 / (pi sqrt(3))
sineFunction = ot.SymbolicFunction(["x"], ["sin(x)"])
sineDistribution = otexp.PushForwardDistribution(
    sineFunction, ot.Uniform(0.0, 2.0 * math.pi)
)
sinePdf = 2.0 / (math.pi * math.sqrt(3.0))
ott.assert_almost_equal(sineDistribution.computePDF([0.5]), sinePdf, 1e-4,
                        0.0)
ott.assert_almost_equal(sineDistribution.computePDF([-0.5]), sinePdf, 1e-4,
                        0.0)
ott.assert_almost_equal(sineDistribution.computePDF([1.5]), 0.0, 0.0, 1e-12)

# Square case in dimension 2: Y = (X0^2, X1^2) with X ~ Normal(2).
# The preimages of (1, 1) are (+/-1, +/-1), each with |det(J)| = 4,
# so that pdf(1, 1) = phi(1)^2
square2Function = ot.SymbolicFunction(["x0", "x1"], ["x0^2", "x1^2"])
square2Distribution = otexp.PushForwardDistribution(square2Function,
                                                    ot.Normal(2))
ott.assert_almost_equal(
    square2Distribution.computePDF([1.0, 1.0]),
    ot.Normal(0.0, 1.0).computePDF([1.0]) ** 2,
    1e-4,
    0.0,
)
ott.assert_almost_equal(square2Distribution.computePDF([-1.0, 1.0]), 0.0,
                        0.0, 1e-6)

# Bounded support: Y = 2X with X ~ Uniform(-1, 1) is Uniform(-2, 2)
uniformDistribution = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x"], ["2.0 * x"]), ot.Uniform(-1.0, 1.0)
)
ott.assert_almost_equal(uniformDistribution.computePDF([0.5]), 0.25, 1e-4, 0.0)
# Points outside the support have a zero density
ott.assert_almost_equal(uniformDistribution.computePDF([3.0]), 0.0, 0.0, 1e-12)
ott.assert_almost_equal(uniformDistribution.computePDF([-3.0]), 0.0, 0.0,
                        1e-12)

# Accessors
fPrime = ot.SymbolicFunction(["x"], ["3.0 * x"])
distribution.setFunction(fPrime)
assert distribution.getFunction() == fPrime
distribution.setAntecedent(ot.Normal(0.0, 1.0))
assert distribution.getAntecedent() == ot.Normal(0.0, 1.0)

# Intrinsic case: p > n with f(x) = (x, x) and X ~ Normal(0, 1).
# The image is the diagonal of R^2 and the density with respect to the
# measure induced on it is f_X(y) / sqrt(2)
diagonalFunction = ot.SymbolicFunction(["x"], ["x", "x"])
diagonalDistribution = otexp.PushForwardDistribution(diagonalFunction,
                                                     ot.Normal(0.0, 1.0))
print("Dimension = ", diagonalDistribution.getDimension())
assert diagonalDistribution.getDimension() == 2
print("diagonal pdf(0, 0)=", diagonalDistribution.computePDF([0.0, 0.0]))
ott.assert_almost_equal(
    diagonalDistribution.computePDF([0.0, 0.0]),
    ot.Normal(0.0, 1.0).computePDF([0.0]) / (2.0 ** 0.5),
    1e-4,
    0.0,
)
print("diagonal pdf(1, 1)=", diagonalDistribution.computePDF([1.0, 1.0]))
ott.assert_almost_equal(
    diagonalDistribution.computePDF([1.0, 1.0]),
    ot.Normal(0.0, 1.0).computePDF([1.0]) / (2.0 ** 0.5),
    1e-4,
    0.0,
)
# Points outside the image have a zero density
ott.assert_almost_equal(diagonalDistribution.computePDF([0.0, 1.0]), 0.0,
                        0.0, 1e-6)

# Default constructor and setters
defaultDistribution = otexp.PushForwardDistribution()
defaultDistribution.setFunction(ot.SymbolicFunction(["x"], ["2.0 * x"]))
defaultDistribution.setAntecedent(ot.Normal(0.0, 1.0))
ott.assert_almost_equal(defaultDistribution.computePDF([0.0]),
                        ot.Normal(0.0, 2.0).computePDF([0.0]))

# Solver accessors
leastSquaresSolver = ot.LeastSquaresEquationsSolver(
    1.0e-5, 1.0e-5, 1.0e-5, 100
)
defaultDistribution.setSolver(leastSquaresSolver)
assert defaultDistribution.getSolver() == leastSquaresSolver
ott.assert_almost_equal(defaultDistribution.computePDF([0.0]),
                        ot.Normal(0.0, 2.0).computePDF([0.0]))

# Exception tests: methods that throw exceptions
with ott.assert_raises(TypeError):
    # the antecedent must be absolutely continuous
    otexp.PushForwardDistribution(f, ot.Bernoulli(0.5))
with ott.assert_raises(TypeError):
    # the function input dimension must match the antecedent dimension
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x"], ["x"]),
                                  ot.Normal(2))
with ott.assert_raises(RuntimeError):
    # the co-area case p < n is not implemented yet
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1"], ["x0"]),
                                  ot.Normal(2))
