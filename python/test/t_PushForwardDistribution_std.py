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
assert distribution.getParameterDescription() == \
    ot.Normal(0.0, 1.0).getParameterDescription()
ott.assert_almost_equal(distribution.getParameter(), [0.0, 1.0], 1e-12, 0.0)
# Setting a parameter of the antecedent changes the pushed-forward distribution:
# X ~ N(0, 4) pushed through f(x) = 2x gives Y ~ N(0, 16)
distribution.setParameter([0.0, 2.0])
assert distribution.getAntecedent() == ot.Normal(0.0, 2.0)
ott.assert_almost_equal(distribution.computePDF([0.0]),
                        1.0 / (4.0 * math.sqrt(2.0 * math.pi)), 1e-4, 0.0)
distribution.setParametersCollection(parameters)
assert distribution.getAntecedent() == ot.Normal(0.0, 1.0)

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

# The co-area case p < n: the fibre f(u) = point is a continuum and the
# density is the integral of the antecedent density along it, divided by the
# p-dimensional Jacobian, i.e. the product of the singular values of the
# p x n Jacobian matrix. The push-forward of the standard normal by the
# projection onto its first coordinates is exactly a normal marginal, which
# an independent distribution object provides as a reference
projection = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0", "x1"])
coArea = otexp.PushForwardDistribution(projection, ot.Normal(3))
marginal = ot.Normal(3).getMarginal([0, 1])
for y in [[0.0, 0.0], [1.0, -0.5], [-1.3, 0.7]]:
    pdf = coArea.computePDF(y)
    print(f"coArea pdf({y})={pdf:.12g}")
    ott.assert_almost_equal(pdf, ot.Normal(2).computePDF(y), 1e-13, 1e-16)
    ott.assert_almost_equal(pdf, marginal.computePDF(y), 1e-13, 1e-16)
ott.assert_almost_equal(coArea.getMean(), [0.0, 0.0], 0.05, 0.05)
assert coArea.getDimension() == 2

# The same in dimension four, to exercise the kernel extraction on a wider
# Jacobian matrix
projection4 = ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["x0", "x1", "x2"])
coArea4 = otexp.PushForwardDistribution(projection4, ot.Normal(4))
ott.assert_almost_equal(coArea4.computePDF([0.4, -0.9, 1.2]),
                        ot.Normal(3).computePDF([0.4, -0.9, 1.2]), 1e-13, 1e-16)

# A non orthogonal linear map: Y = A X is Normal(0, A A^T), so the reference
# does not rely on the co-area formula at all. The Jacobian is sqrt(det(A A^T))
# = sqrt(19), a factor the density is divided by
aMatrix = ot.Matrix([[1.0, 1.0, 0.0], [1.0, 2.0, 3.0]])
linear = ot.LinearFunction(ot.Point(3), ot.Point(2), aMatrix)
linearCoArea = otexp.PushForwardDistribution(linear, ot.Normal(3))
covariance = ot.CovarianceMatrix([[2.0, 3.0], [3.0, 14.0]])
linearReference = ot.Normal(ot.Point(2), covariance)
for y in [[0.0, 0.0], [0.8, -1.1]]:
    pdf = linearCoArea.computePDF(y)
    print(f"linearCoArea pdf({y})={pdf:.12g}")
    ott.assert_almost_equal(pdf, linearReference.computePDF(y), 1e-13, 1e-16)

# A heavy tailed antecedent: the extension of the fibre beyond the search box
# stops as soon as the integrand is negligible there, so that the density is
# not diluted by integrating a long interval where the quadrature would miss
# the bulk of the mass
student = ot.Student(4.0)
studentCoArea = otexp.PushForwardDistribution(
    projection, ot.JointDistribution([student] * 3))
for y in [[0.0, 0.0], [1.0, -0.5]]:
    pdf = studentCoArea.computePDF(y)
    print(f"studentCoArea pdf({y})={pdf:.12g}")
    reference = student.computePDF(y[0]) * student.computePDF(y[1])
    ott.assert_almost_equal(pdf, reference, 1e-12, 1e-16)

# A bounded antecedent: the image of the box is uniform on the projected box,
# the density jumping at the bounds of its support
box = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3)
boxCoArea = otexp.PushForwardDistribution(projection, box)
for y in [[0.0, 0.0], [0.3, -0.4], [0.99, 0.99]]:
    ott.assert_almost_equal(boxCoArea.computePDF(y), 0.25, 1e-13, 1e-16)
# A point outside the image of the box has an empty fibre, hence a zero density
assert boxCoArea.computePDF([1.7, 0.0]) == 0.0
assert boxCoArea.computePDF([0.0, -1.5]) == 0.0

# A degenerate map, whose image is a line of R^2: the density is singular on
# the image, and vanishes outside of it
degenerate = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0", "0.0"])
degenerateCoArea = otexp.PushForwardDistribution(degenerate, ot.Normal(3))
assert math.isinf(degenerateCoArea.computePDF([0.5, 0.0]))
assert degenerateCoArea.computePDF([0.5, 1.0]) == 0.0

# Exception tests: methods that throw exceptions
with ott.assert_raises(TypeError):
    # the antecedent must be absolutely continuous
    otexp.PushForwardDistribution(f, ot.Bernoulli(0.5))
with ott.assert_raises(TypeError):
    # the function input dimension must match the antecedent dimension
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x"], ["x"]),
                                  ot.Normal(2))
with ott.assert_raises(RuntimeError):
    # a fibre of dimension two is not implemented yet
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1", "x2"], ["x0"]),
                                  ot.Normal(3)).computePDF([0.0])
with ott.assert_raises(RuntimeError):
    # x0 + x1^2 has a parabolic fibre, the straight parameterisation of the
    # fibre is invalid, so the density is refused instead of being wrong
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1"],
                                                      ["x0 + x1^2"]),
                                  ot.Normal(2)).computePDF([1.0])
with ott.assert_raises(RuntimeError):
    # f(x0, x1, x2) = (x0, x1^2) is not affine: the Jacobian at the mean is
    # singular, and the point is outside the image of the linearization. The
    # density must be refused instead of being reported as zero, while the
    # density of the push-forward is positive there
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1", "x2"],
                                                      ["x0", "x1^2"]),
                                  ot.Normal(3)).computePDF([0.5, 1.0])
with ott.assert_raises(RuntimeError):
    # Same function, but the point belongs to the image of the linearization,
    # where the singular Jacobian would report an infinite density while the
    # density of the push-forward is finite
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1", "x2"],
                                                      ["x0", "x1^2"]),
                                  ot.Normal(3)).computePDF([0.5, 0.0])
with ott.assert_raises(RuntimeError):
    # f(x0, x1, x2) = (x0^2, x1) is not affine either: its Jacobian vanishes at
    # the mean, and the point is outside the image of the linearization
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x0", "x1", "x2"],
                                                      ["x0^2", "x1"]),
                                  ot.Normal(3)).computePDF([1.0, 0.0])
