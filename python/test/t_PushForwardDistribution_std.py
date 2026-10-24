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

# Affine moments are analytical, not Monte-Carlo estimates: for Y = 2 X
# with X ~ Normal(0, 1), the mean is 0 and the standard deviation is 2
mean = distribution.getMean()
print("mean=", mean)
ott.assert_almost_equal(mean, reference.getMean(), 1e-14, 1e-14)
standardDeviation = distribution.getStandardDeviation()
print("standard deviation=", standardDeviation)
ott.assert_almost_equal(standardDeviation,
                        reference.getStandardDeviation(), 1e-14, 1e-14)

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
# Y = X^2 with X ~ Normal(0, 1) has mean 1 and variance E[X^4] - 1 = 2.
# The moments are deterministic quadrature, not Monte-Carlo estimates
ott.assert_almost_equal(squareDistribution.getMean(), [1.0], 1e-8, 1e-12)
ott.assert_almost_equal(squareDistribution.getCovariance(),
                        ot.CovarianceMatrix(1, [2.0]), 1e-8, 1e-12)

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
ott.assert_almost_equal(coArea.getMean(), [0.0, 0.0], 1e-14, 1e-14)
ott.assert_almost_equal(coArea.getCovariance(), ot.CovarianceMatrix(2),
                        1e-14, 1e-14)
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
ott.assert_almost_equal(linearCoArea.getMean(), linearReference.getMean(),
                        1e-14, 1e-14)
ott.assert_almost_equal(linearCoArea.getCovariance(),
                        linearReference.getCovariance(), 1e-14, 1e-14)

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

# Multidimensional fibres (n - p >= 2): the fibre is a polytope obtained by
# intersecting the affine fibre with the search box, and its nested interval
# description is built from the exact projections of the polytope. The
# reference densities below come from independent distribution objects, not
# from the co-area formula.

# d = 2 with a bounded support: Y = X0 + X1 + X2 with independent
# Uniform(-1, 1) components
uniformSumFunction = ot.SymbolicFunction(["x0", "x1", "x2"],
                                         ["x0 + x1 + x2"])
uniformSumDistribution = otexp.PushForwardDistribution(
    uniformSumFunction, ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3))
uniformSumReference = ot.LinearCombinationDistribution(
    [ot.Uniform(-1.0, 1.0)] * 3, [1.0, 1.0, 1.0])
for y in [0.0, 0.5, 1.0]:
    pdf = uniformSumDistribution.computePDF([y])
    print(f"uniformSum pdf({y})={pdf:.12g}")
    ott.assert_almost_equal(pdf, uniformSumReference.computePDF([y]), 1e-12,
                            1e-15)
# The fibre is empty outside the support of the sum
assert uniformSumDistribution.computePDF([4.0]) == 0.0

# d = 2 with a heavy tailed support: the projection of three independent
# Student(4) variables on their first coordinate is a Student(4) variable,
# whatever the two free coordinates
studentProjection = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0"])
studentProjectionDistribution = otexp.PushForwardDistribution(
    studentProjection, ot.JointDistribution([student] * 3))
pdf = studentProjectionDistribution.computePDF([2.0])
print(f"studentProjection pdf(2)={pdf:.12g}")
ott.assert_almost_equal(pdf, student.computePDF([2.0]), 1e-6, 0.0)

# d = 2 with a rotated kernel: the sum of two independent Student(4)
# variables, the third coordinate being free
studentSumFunction = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0 + x1"])
studentSumDistribution = otexp.PushForwardDistribution(
    studentSumFunction, ot.JointDistribution([student] * 3))
studentSumReference = ot.LinearCombinationDistribution(
    [student, student], [1.0, 1.0])
pdf = studentSumDistribution.computePDF([1.0])
print(f"studentSum pdf(1)={pdf:.12g}")
ott.assert_almost_equal(pdf, studentSumReference.computePDF([1.0]), 1e-6, 0.0)

# d = 2 with a non axis aligned kernel: Y = X0 + 2 X1 + 3 X2 with independent
# standard normal components is Normal(0, sqrt(14))
weightedSum = ot.LinearFunction(ot.Point(3), ot.Point(1),
                                ot.Matrix(1, 3, [1.0, 2.0, 3.0]))
weightedSumDistribution = otexp.PushForwardDistribution(weightedSum,
                                                        ot.Normal(3))
pdf = weightedSumDistribution.computePDF([0.5])
print(f"weightedSum pdf(0.5)={pdf:.12g}")
ott.assert_almost_equal(pdf, ot.Normal(0.0, math.sqrt(14.0)).computePDF([0.5]),
                        1e-10, 1e-15)

# d = 2 with a two dimensional image and a non orthogonal Jacobian:
# Y = (X0 + X1, X1 + X2) with X ~ Normal(4) is Normal(0, A A^T)
linearMatrix = ot.Matrix(2, 4, [1.0, 1.0, 0.0, 0.0,
                                0.0, 1.0, 1.0, 0.0])
linearFunction = ot.LinearFunction(ot.Point(4), ot.Point(2), linearMatrix)
linearDistribution = otexp.PushForwardDistribution(linearFunction,
                                                   ot.Normal(4))
linearReference = ot.Normal(
    ot.Point(2), ot.CovarianceMatrix([[2.0, 1.0], [1.0, 2.0]]))
for y in [[0.0, 0.0], [0.5, -0.7]]:
    pdf = linearDistribution.computePDF(y)
    print(f"linear4 pdf({y})={pdf:.12g}")
    ott.assert_almost_equal(pdf, linearReference.computePDF(y), 1e-10, 1e-15)

# d = 3: the mean of four independent normal variables is Normal(0, 1/2)
meanFourFunction = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"], ["0.25 * (x0 + x1 + x2 + x3)"])
meanFourDistribution = otexp.PushForwardDistribution(
    meanFourFunction, ot.JointDistribution([ot.Normal()] * 4))
pdf = meanFourDistribution.computePDF([0.5])
print(f"meanFour pdf(0.5)={pdf:.12g}")
ott.assert_almost_equal(pdf, ot.Normal(0.0, 0.5).computePDF([0.5]), 1e-10,
                        1e-15)

# d = 3 with a bounded support: the sum of four independent Uniform(-1, 1)
# variables. The kinks of the nested bounds limit the adaptive quadrature to
# about 1e-7, so the requested error is relaxed for this case to keep the
# test fast
uniformSumFourFunction = ot.SymbolicFunction(
    ["x0", "x1", "x2", "x3"], ["x0 + x1 + x2 + x3"])
uniformSumFourReference = ot.LinearCombinationDistribution(
    [ot.Uniform(-1.0, 1.0)] * 4, [1.0, 1.0, 1.0, 1.0])
previousError = ot.ResourceMap.GetAsScalar(
    "PushForwardDistribution-CoAreaMaximumError")
ot.ResourceMap.SetAsScalar("PushForwardDistribution-CoAreaMaximumError", 1.0e-6)
try:
    uniformSumFourDistribution = otexp.PushForwardDistribution(
        uniformSumFourFunction,
        ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 4))
    pdf = uniformSumFourDistribution.computePDF([0.0])
finally:
    ot.ResourceMap.SetAsScalar(
        "PushForwardDistribution-CoAreaMaximumError", previousError)
print(f"uniformSumFour pdf(0)={pdf:.12g}")
ott.assert_almost_equal(pdf, uniformSumFourReference.computePDF([0.0]), 1e-6,
                        0.0)

# The floor of the requested error keeps the cost reasonable for a fibre of
# dimension three even when the default maximum error asks for much more
# accuracy than the nested rule can deliver across the kinks of its bounds
previousFloor = ot.ResourceMap.GetAsScalar(
    "PushForwardDistribution-CoAreaErrorFloor")
ot.ResourceMap.SetAsScalar("PushForwardDistribution-CoAreaErrorFloor", 1.0e-4)
try:
    pdf = uniformSumFourDistribution.computePDF([0.0])
finally:
    ot.ResourceMap.SetAsScalar(
        "PushForwardDistribution-CoAreaErrorFloor", previousFloor)
print(f"uniformSumFour floored pdf(0)={pdf:.12g}")
ott.assert_almost_equal(pdf, uniformSumFourReference.computePDF([0.0]), 1e-3,
                        0.0)

# d = 4: the mean of five independent normal variables, with the requested
# error relaxed to keep the test fast
meanFiveFunction = ot.LinearFunction(ot.Point(5), ot.Point(1),
                                     ot.Matrix(1, 5, [0.2] * 5))
ot.ResourceMap.SetAsScalar("PushForwardDistribution-CoAreaMaximumError", 1.0e-3)
try:
    meanFiveDistribution = otexp.PushForwardDistribution(
        meanFiveFunction, ot.JointDistribution([ot.Normal()] * 5))
    pdf = meanFiveDistribution.computePDF([0.5])
finally:
    ot.ResourceMap.SetAsScalar(
        "PushForwardDistribution-CoAreaMaximumError", previousError)
print(f"meanFive pdf(0.5)={pdf:.12g}")
ott.assert_almost_equal(pdf, ot.Normal(0.0, 1.0 / math.sqrt(5.0)).computePDF([0.5]),
                        1e-6, 0.0)

# A degenerate map, whose image is a line of R^2: the density is singular on
# the image, and vanishes outside of it
degenerate = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0", "0.0"])
degenerateCoArea = otexp.PushForwardDistribution(degenerate, ot.Normal(3))
assert math.isinf(degenerateCoArea.computePDF([0.5, 0.0]))
assert degenerateCoArea.computePDF([0.5, 1.0]) == 0.0

# A rank-deficient affine map with a bounded antecedent: the algebraic image
# is a line, but a point on it may still have no preimage in the support
boundedDegenerate = otexp.PushForwardDistribution(
    degenerate, ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3))
assert math.isinf(boundedDegenerate.computePDF([0.5, 0.0]))
assert boundedDegenerate.computePDF([100.0, 0.0]) == 0.0

# The minimum-norm point of the fibre may lie outside the support while
# another point of the same fibre lies inside it
shiftedDegenerate = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0 + x1", "0.0"]),
    ot.JointDistribution([ot.Uniform(0.0, 10.0),
                          ot.Uniform(0.0, 1.0),
                          ot.Uniform(0.0, 1.0)]))
assert math.isinf(shiftedDegenerate.computePDF([10.5, 0.0]))
assert shiftedDegenerate.computePDF([12.0, 0.0]) == 0.0

# Exception tests: methods that throw exceptions
with ott.assert_raises(TypeError):
    # the antecedent must be absolutely continuous
    otexp.PushForwardDistribution(f, ot.Bernoulli(0.5))
with ott.assert_raises(TypeError):
    # the function input dimension must match the antecedent dimension
    otexp.PushForwardDistribution(ot.SymbolicFunction(["x"], ["x"]),
                                  ot.Normal(2))
# A nonlinear scalar map on R^2 has a one dimensional fibre: its density is
# supported by the triangulated level-set extraction, and the straight affine
# path is not used because isLinear() is false. Y = X0^2 with X ~ Normal(2) is
# the chi-square distribution with one degree of freedom
nonlinearCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1"], ["x0^2"]), ot.Normal(2)
)
assert nonlinearCoArea.getDimension() == 1
assert nonlinearCoArea.getSample(10).getSize() == 10
ott.assert_almost_equal(nonlinearCoArea.getMean(), [1.0], 1e-8, 1e-12)
ott.assert_almost_equal(nonlinearCoArea.getCovariance(),
                        ot.CovarianceMatrix(1, [2.0]), 1e-8, 1e-12)
for level in [0.7, 1.0]:
    expected = ot.Normal().computePDF([math.sqrt(level)]) / math.sqrt(level)
    pdf = nonlinearCoArea.computePDF([level])
    print(f"nonlinearCoArea pdf({level})={pdf:.12g}")
    ott.assert_almost_equal(pdf, expected, 2e-3, 0.0)
ott.assert_almost_equal(math.exp(nonlinearCoArea.computeLogPDF([1.0])),
                        ot.Normal().computePDF([1.0]), 1e-6, 0.0)

# A function without an analytical gradient exercises the P1 fallback used by
# the nonlinear co-area factor
pythonNonlinear = otexp.PushForwardDistribution(
    ot.PythonFunction(2, 1, lambda x: [x[0] ** 2]), ot.Normal(2)
)
ott.assert_almost_equal(pythonNonlinear.computePDF([1.0]),
                        ot.Normal().computePDF([1.0]), 1e-3, 0.0)

# Y = X0^2 + X1^2 with X ~ Normal(2) is the exponential distribution of
# rate 1/2: its density is 0.5 * exp(-y / 2) for y > 0
radialCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1^2"]), ot.Normal(2)
)
for level in [0.5, 1.0, 2.0]:
    expected = 0.5 * math.exp(-0.5 * level)
    pdf = radialCoArea.computePDF([level])
    print(f"radialCoArea pdf({level})={pdf:.12g}")
    ott.assert_almost_equal(pdf, expected, 2e-3, 0.0)

# The continuation key refines the closed fibre of the previous map. The
# mesh result remains the fallback when the key is disabled.
continuationKey = "PushForwardDistribution-NonlinearCoAreaContinuation"
levels = [0.5, 1.0, 2.0]
ot.ResourceMap.SetAsBool(continuationKey, False)
radialMeshPDF = [radialCoArea.computePDF([level]) for level in levels]
ot.ResourceMap.SetAsBool(continuationKey, True)
for level, meshPDF in zip(levels, radialMeshPDF):
    expected = 0.5 * math.exp(-0.5 * level)
    continuationPDF = radialCoArea.computePDF([level])
    print(f"radialCoArea continuation pdf({level})={continuationPDF:.12g}")
    ott.assert_almost_equal(continuationPDF, expected, 1e-4, 0.0)
    assert abs(continuationPDF - expected) < abs(meshPDF - expected)

# An open scalar fibre exercises the two directions of the continuation: the
# two branches of Y = X1^2 are horizontal lines, each one contributes
# phi(1) / 2 because the co-area factor is 2 * |x1| = 2, so the total is phi(1).
openCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1"], ["x1^2"]), ot.Normal(2)
)
ot.ResourceMap.SetAsBool(continuationKey, False)
openMeshPDF = openCoArea.computePDF([1.0])
ot.ResourceMap.SetAsBool(continuationKey, True)
openContinuationPDF = openCoArea.computePDF([1.0])
expected = ot.Normal().computePDF([1.0])
print(f"openCoArea continuation pdf(1.0)={openContinuationPDF:.12g}")
ott.assert_almost_equal(openContinuationPDF, expected, 1e-6, 0.0)
assert abs(openContinuationPDF - expected) < abs(openMeshPDF - expected)

# A scalar nonlinear map on R^3 has a two dimensional fibre: the level set is
# extracted from a tetrahedralisation of the search box, and the density is
# integrated over the resulting triangulated surface. Y = X0^2 + X1^2 + X2^2
# with X ~ Normal(3) is the chi-square distribution with three degrees of
# freedom.
surfaceCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2 + x2^2"]),
    ot.Normal(3),
)
assert surfaceCoArea.getDimension() == 1
chiSquare3 = ot.ChiSquare(3)
for level in [0.5, 1.0, 2.0]:
    expected = chiSquare3.computePDF([level])
    pdf = surfaceCoArea.computePDF([level])
    print(f"surfaceCoArea pdf({level})={pdf:.12g}")
    ott.assert_almost_equal(pdf, expected, 2e-2, 0.0)
# A negative level is outside the image of the sum of squares: its fibre is
# empty and the density is zero.
assert surfaceCoArea.computePDF([-1.0]) == 0.0
# The deterministic moments are still obtained by input-space quadrature.
ott.assert_almost_equal(surfaceCoArea.getMean(), [3.0], 1e-8, 1e-12)
ott.assert_almost_equal(
    surfaceCoArea.getCovariance(), ot.CovarianceMatrix(1, [6.0]), 1e-8, 1e-12
)

# A level surface that coincides with a mesh plane exercises the face
# deduplication: Y = X0^2 on R^3 has two flat branes x0 = +/- sqrt(y), and its
# density is the chi-square distribution with one degree of freedom.
surfaceDiscretizationKey = "PushForwardDistribution-NonlinearCoAreaSurfaceDiscretization"
previousSurfaceDiscretization = ot.ResourceMap.GetAsUnsignedInteger(
    surfaceDiscretizationKey
)
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 24)
planeCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2"]), ot.Normal(3)
)
for level in [0.7, 1.0]:
    expected = ot.Normal().computePDF([math.sqrt(level)]) / math.sqrt(level)
    pdf = planeCoArea.computePDF([level])
    print(f"planeCoArea pdf({level})={pdf:.12g}")
    ott.assert_almost_equal(pdf, expected, 2e-3, 0.0)
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A level surface whose triangles are shared by many tetrahedra exercises the
# global vertex numbering used by the face deduplication.  The level x2 = 0 of
# f(x) = x2 exp(x0) is a mesh plane, and its density is
# phi(0) E[exp(-X0)] = phi(0) exp(1 / 2).
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 16)
faceDedup = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x2 * exp(x0)"]), ot.Normal(3)
)
expectedFaceDedup = ot.Normal().computePDF(0.0) * math.exp(0.5)
pdf = faceDedup.computePDF([0.0])
print(f"faceDedup pdf(0)={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedFaceDedup, 1e-6, 0.0)
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A scalar nonlinear map on R^3 without an analytical gradient uses the
# finite difference fallback for the co-area factor.
pythonSurface = otexp.PushForwardDistribution(
    ot.PythonFunction(3, 1, lambda x: [x[0] ** 2 + x[1] ** 2 + x[2] ** 2]),
    ot.Normal(3),
)
ott.assert_almost_equal(pythonSurface.computePDF([1.0]),
                        chiSquare3.computePDF([1.0]), 2e-2, 0.0)

# A nonlinear map from R^3 to R^2 has a one dimensional fibre, the
# intersection of the two level surfaces. For the uniform cube and
# Y = (X0^2 + X1^2, X2), the fibre at (r^2, y1) with 0 < r < 1 is the
# quarter circle of radius r in the positive quadrant. Its length is
# pi * r / 2, the two dimensional Jacobian is 2 * r, and the antecedent
# density is one, so the density is pi / 4.
uniformCube = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 3)
ot.ResourceMap.SetAsUnsignedInteger(surfaceDiscretizationKey, 24)
curveCoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2", "x2"]),
    uniformCube,
)
for point in [[0.5, 0.5], [0.2, 0.3]]:
    pdf = curveCoArea.computePDF(point)
    print(f"curveCoArea pdf({point})={pdf:.12g}")
    ott.assert_almost_equal(pdf, math.pi / 4.0, 1e-3, 0.0)
ot.ResourceMap.SetAsBool(continuationKey, False)
curveMeshPDF = [curveCoArea.computePDF(point) for point in [[0.5, 0.5], [0.2, 0.3]]]
ot.ResourceMap.SetAsBool(continuationKey, True)
for point, meshPDF in zip([[0.5, 0.5], [0.2, 0.3]], curveMeshPDF):
    continuationPDF = curveCoArea.computePDF(point)
    print(f"curveCoArea continuation pdf({point})={continuationPDF:.12g}")
    ott.assert_almost_equal(continuationPDF, math.pi / 4.0, 1e-5, 0.0)
    assert abs(continuationPDF - math.pi / 4.0) < abs(meshPDF - math.pi / 4.0)

# Points outside the image of the two components have an empty fibre.
assert curveCoArea.computePDF([-0.1, 0.5]) == 0.0
assert curveCoArea.computePDF([0.5, 1.5]) == 0.0

# With independent standard normal inputs, the same map has the closed-form
# density 0.5 * exp(-y0 / 2) * phi(y1), which also exercises the finite
# search box and the non-constant antecedent density.
normalCurve = otexp.PushForwardDistribution(
    ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2 + x1^2", "x2"]),
    ot.Normal(3),
)
curvePoint = [0.7, 0.4]
expectedCurvePDF = (0.5 * math.exp(-0.5 * curvePoint[0])
                    * ot.Normal().computePDF([curvePoint[1]]))
pdf = normalCurve.computePDF(curvePoint)
print(f"normalCurve pdf({curvePoint})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedCurvePDF, 1e-2, 0.0)
# The deterministic moments are still obtained by input-space quadrature.
ott.assert_almost_equal(normalCurve.getMean(), [2.0, 0.0], 1e-8, 1e-9)
ott.assert_almost_equal(
    normalCurve.getCovariance(),
    ot.CovarianceMatrix(2, [4.0, 0.0, 0.0, 1.0]),
    1e-8,
    1e-9,
)
ot.ResourceMap.SetAsUnsignedInteger(
    surfaceDiscretizationKey, previousSurfaceDiscretization
)

# A nonlinear map from R^4 to R^3 has a one dimensional fibre as well. For
# the uniform cube and Y = (X0^2 + X1^2, X2, X3), the fibre at (r^2, y1, y2)
# is a quarter circle of radius r, so the density is again pi / 4.
volumeKey = "PushForwardDistribution-NonlinearCoAreaVolumeDiscretization"
previousVolumeDiscretization = ot.ResourceMap.GetAsUnsignedInteger(volumeKey)
ot.ResourceMap.SetAsUnsignedInteger(volumeKey, 8)
uniformCube4 = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * 4)
curve4CoArea = otexp.PushForwardDistribution(
    ot.SymbolicFunction(
        ["x0", "x1", "x2", "x3"],
        ["x0^2 + x1^2", "x2", "x3"],
    ),
    uniformCube4,
)
curve4Points = [[0.5, 0.5, 0.5], [0.2, 0.3, 0.4]]
for point in curve4Points:
    pdf = curve4CoArea.computePDF(point)
    print(f"curve4CoArea pdf({point})={pdf:.12g}")
    ott.assert_almost_equal(pdf, math.pi / 4.0, 1e-5, 0.0)
ot.ResourceMap.SetAsBool(continuationKey, False)
curve4MeshPDF = [curve4CoArea.computePDF(point) for point in curve4Points]
ot.ResourceMap.SetAsBool(continuationKey, True)
for point, meshPDF in zip(curve4Points, curve4MeshPDF):
    continuationPDF = curve4CoArea.computePDF(point)
    print(f"curve4CoArea continuation pdf({point})={continuationPDF:.12g}")
    ott.assert_almost_equal(continuationPDF, math.pi / 4.0, 1e-5, 0.0)
    assert abs(continuationPDF - math.pi / 4.0) < abs(meshPDF - math.pi / 4.0)

# An empty R^4 fibre has a zero density.
assert curve4CoArea.computePDF([-0.1, 0.5, 0.5]) == 0.0
assert curve4CoArea.computePDF([0.5, 1.5, 0.5]) == 0.0
assert curve4CoArea.computePDF([0.5, 0.5, 1.5]) == 0.0

# Restore the default volume discretization before the normal R^4 tests.
ot.ResourceMap.SetAsUnsignedInteger(volumeKey, previousVolumeDiscretization)

# With independent standard normal inputs, the R^4 -> R^3 density is
# 0.5 * exp(-y0 / 2) * phi(y1) * phi(y2), which exercises the finite search
# box and the non-constant antecedent density in dimension four.
normalCurve4 = otexp.PushForwardDistribution(
    ot.SymbolicFunction(
        ["x0", "x1", "x2", "x3"],
        ["x0^2 + x1^2", "x2", "x3"],
    ),
    ot.Normal(4),
)
curve4Point = [0.7, 0.4, 0.5]
expectedCurve4PDF = (
    0.5
    * math.exp(-0.5 * curve4Point[0])
    * ot.Normal().computePDF([curve4Point[1]])
    * ot.Normal().computePDF([curve4Point[2]])
)
pdf = normalCurve4.computePDF(curve4Point)
print(f"normalCurve4 pdf({curve4Point})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedCurve4PDF, 1e-4, 0.0)
# A fibre whose radius is smaller than the mesh step exercises the closure of
# a short loop: it must be traced once, not twice.
tinyCurve4Point = [0.02, 0.0, 0.0]
expectedTinyCurve4PDF = (
    0.5
    * math.exp(-0.5 * tinyCurve4Point[0])
    * ot.Normal().computePDF([tinyCurve4Point[1]])
    * ot.Normal().computePDF([tinyCurve4Point[2]])
)
pdf = normalCurve4.computePDF(tinyCurve4Point)
print(f"normalCurve4 pdf({tinyCurve4Point})={pdf:.12g}")
ott.assert_almost_equal(pdf, expectedTinyCurve4PDF, 1e-4, 0.0)

# A nonlinear R^3 -> R^2 map without an analytical gradient uses the finite
# difference fallback for the two dimensional Jacobian.
pythonCurve = otexp.PushForwardDistribution(
    ot.PythonFunction(3, 2, lambda x: [x[0] ** 2 + x[1] ** 2, x[2]]),
    uniformCube,
)
ott.assert_almost_equal(pythonCurve.computePDF([0.5, 0.4]),
                        math.pi / 4.0, 2e-3, 0.0)

# Nonlinear co-area maps with a higher dimensional fibre are still accepted
# at construction, and their PDF fails with an explicit message.
nonAffineFunctions = [
    (ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["x0 + x1^2"]),
     ot.Normal(4)),
    (ot.SymbolicFunction(["x0", "x1", "x2", "x3"], ["x0^2", "x1"]),
     ot.Normal(4)),
]
for nonAffineFunction, nonAffineAntecedent in nonAffineFunctions:
    nonAffineDistribution = otexp.PushForwardDistribution(
        nonAffineFunction, nonAffineAntecedent
    )
    outputDimension = nonAffineFunction.getOutputDimension()
    assert nonAffineDistribution.getDimension() == outputDimension
    assert nonAffineDistribution.getSample(3).getSize() == 3
    with ott.assert_raises(RuntimeError):
        nonAffineDistribution.computePDF(ot.Point(outputDimension))

# Above the configured dimension, the deterministic iterated quadrature is
# replaced by a Monte-Carlo estimate.  The map X0^2 + X1 with two standard
# normals has mean 1 and variance 3.
momentsDimensionKey = "PushForwardDistribution-MomentsMaximumDimension"
previousMomentsDimension = ot.ResourceMap.GetAsUnsignedInteger(momentsDimensionKey)
try:
    ot.ResourceMap.SetAsUnsignedInteger(momentsDimensionKey, 1)
    sampledMoments = otexp.PushForwardDistribution(
        ot.SymbolicFunction(["x0", "x1"], ["x0^2 + x1"]), ot.Normal(2)
    )
    ott.assert_almost_equal(sampledMoments.getMean(), [1.0], 0.0, 0.2)
    ott.assert_almost_equal(
        sampledMoments.getCovariance()[0, 0], 3.0, 0.0, 0.4
    )
finally:
    ot.ResourceMap.SetAsUnsignedInteger(
        momentsDimensionKey, previousMomentsDimension
    )
