#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
from math import sqrt, pi, exp, log

ot.TESTPREAMBLE()

ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 4000000
)
# Deactivate the simplification mechanism as we want to test the Poisson formula
# based algorithm here
ot.ResourceMap.SetAsBool("LinearCombinationDistribution-SimplifyAtoms", False)

# Create a collection of test-cases and the associated references
testCases = list()
references = list()
testCases.append([ot.Uniform(-1.0, 3.0)] * 2)
references.append(ot.Triangular(-2.0, 2.0, 6.0))
testCases.append([ot.Normal(), ot.Normal(1.0, 2.0), ot.Normal(-2.0, 2.0)])
references.append(ot.Normal(-1.0, 3.0))
testCases.append([ot.Exponential()] * 3)
references.append(ot.Gamma(3.0, 1.0, 0.0))
print("testCases=", testCases)

size = 10000
for testIndex in range(len(testCases)):
    # Instantiate one distribution object
    distribution = ot.LinearCombinationDistribution(testCases[testIndex])
    distribution.setBlockMin(5)
    distribution.setBlockMax(20)
    distributionReference = references[testIndex]

    # Test for realization of distribution
    oneRealization = distribution.getRealization()
    assert oneRealization.getDimension() == distribution.getDimension()
    assert distribution.getRange().contains(oneRealization)

    # Test for sampling: check the sample statistics against the reference
    oneSample = distribution.getSample(size)
    ott.assert_almost_equal(
        oneSample.computeMean(),
        distributionReference.getMean(),
        0.1,
        0.0,
        "sample mean",
    )
    ott.assert_almost_equal(
        oneSample.computeStandardDeviation(),
        distributionReference.getStandardDeviation(),
        0.1,
        0.0,
        "sample standard deviation",
    )

    # Define a point
    point = ot.Point(distribution.getDimension(), 0.5)

    # Check the PDF, CDF and DDF against the reference distribution
    # Tolerance accounts for the truncation of the Fourier sum far from the
    # center once the confidence band has been adapted
    ott.assert_almost_equal(
        distribution.computeDDF(point),
        distributionReference.computeDDF(point),
        5e-3,
        1e-8,
        "ddf",
    )
    ott.assert_almost_equal(
        distribution.computePDF(point),
        distributionReference.computePDF(point),
        5e-3,
        1e-8,
        "pdf",
    )
    ott.assert_almost_equal(
        distribution.computeCDF(point),
        distributionReference.computeCDF(point),
        5e-3,
        1e-7,
        "cdf",
    )

    # Check the characteristic function against the reference
    CF = distribution.computeCharacteristicFunction(point[0])
    LCF = distribution.computeLogCharacteristicFunction(point[0])
    ott.assert_almost_equal(
        CF,
        distributionReference.computeCharacteristicFunction(point[0]),
        1e-5,
        1e-8,
        "characteristic function",
    )
    ott.assert_almost_equal(
        LCF,
        distributionReference.computeLogCharacteristicFunction(point[0]),
        1e-5,
        1e-8,
        "log characteristic function",
    )

    # Check the quantiles
    quantile = distribution.computeQuantile(0.95)
    ott.assert_almost_equal(
        distribution.computeCDF(quantile), 0.95, 1e-7, 1e-9, "quantile"
    )
    ott.assert_almost_equal(
        quantile, distributionReference.computeQuantile(0.95), 1e-3, 1e-8
    )
    quantileComp = distribution.computeQuantile(0.95, True)
    ott.assert_almost_equal(
        distribution.computeComplementaryCDF(quantileComp),
        0.95,
        1e-7,
        1e-9,
        "complementary quantile",
    )
    ott.assert_almost_equal(
        distribution.computeCDF(quantileComp), 0.05, 1e-7, 1e-9
    )

    # Get 95% survival function
    inverseSurvival = ot.Point(distribution.computeInverseSurvivalFunction(0.95))
    ott.assert_almost_equal(
        distribution.computeSurvivalFunction(inverseSurvival),
        0.95,
        1e-7,
        1e-9,
        "inverse survival",
    )

    mean = distribution.getMean()
    standardDeviation = distribution.getStandardDeviation()
    skewness = distribution.getSkewness()
    kurtosis = distribution.getKurtosis()
    covariance = distribution.getCovariance()
    ott.assert_almost_equal(mean, distributionReference.getMean(), 1e-10, 1e-12, "mean")
    ott.assert_almost_equal(
        standardDeviation,
        distributionReference.getStandardDeviation(),
        1e-10,
        1e-12,
        "standard deviation",
    )
    ott.assert_almost_equal(
        skewness, distributionReference.getSkewness(), 1e-10, 1e-12, "skewness"
    )
    ott.assert_almost_equal(
        kurtosis, distributionReference.getKurtosis(), 1e-10, 1e-12, "kurtosis"
    )
    ott.assert_almost_equal(
        covariance, distributionReference.getCovariance(), 1e-10, 1e-12, "covariance"
    )

    parameters = distribution.getParametersCollection()
    assert len(parameters) > 0, "empty parameter collection"

    assert distribution.getBlockMin() == 5, "wrong blockMin"
    assert distribution.getBlockMax() == 20, "wrong blockMax"
    assert (
        distribution.getMaxSize()
        == ot.ResourceMap.GetAsUnsignedInteger(
            "LinearCombinationDistribution-DefaultMaxSize"
        )
    ), "wrong maxSize"
    ott.assert_almost_equal(
        distribution.getAlpha(),
        ot.ResourceMap.GetAsScalar("LinearCombinationDistribution-DefaultAlpha"),
    )
    ott.assert_almost_equal(
        distribution.getBeta(),
        ot.ResourceMap.GetAsScalar("LinearCombinationDistribution-DefaultBeta"),
    )

    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipEntropy()  # slow
    validation.skipMinimumVolumeLevelSet()  # slow
    validation.skipTransformation()  # transformation accuracy is a bit low
    validation.run()

# Tests of the simplification mechanism: check the PDF of a mixture involving
# many kinds of atoms against anchored values
weights = ot.Point(0)
coll = ot.DistributionCollection(0)
coll.add(ot.Dirac(0.5))
weights.add(1.0)
coll.add(ot.Normal(1.0, 2.0))
weights.add(2.0)
coll.add(ot.Normal(2.0, 1.0))
weights.add(-3.0)
coll.add(ot.Uniform(-2.0, 2.0))
weights.add(-1.0)
coll.add(ot.Uniform(2.0, 4.0))
weights.add(2.0)
coll.add(ot.Exponential(2.0, -3.0))
weights.add(1.5)
rm = ot.LinearCombinationDistribution(coll, weights)
coll.add(rm)
weights.add(-2.5)
coll.add(ot.Gamma(3.0, 4.0, -2.0))
weights.add(2.5)
distribution = ot.LinearCombinationDistribution(coll, weights)
print("distribution=", distribution)
mu = distribution.getMean()[0]
sigma = distribution.getStandardDeviation()[0]
anchoredPDF = [
    0.000311,
    0.001834,
    0.006936,
    0.016849,
    0.026283,
    0.026313,
    0.016889,
    0.006940,
    0.001821,
    0.000305,
]
for i in range(10):
    x = mu + (-3.0 + 6.0 * i / 9.0) * sigma
    pdf = distribution.computePDF(x)
    ott.assert_almost_equal(pdf, anchoredPDF[i], 1e-3, 1e-6, "anchored pdf")
    ott.assert_almost_equal(
        distribution.computeLogPDF(x), log(pdf), 1e-6, 1e-8, "log pdf"
    )

# Tests of the projection mechanism
collFactories = [
    ot.UniformFactory(),
    ot.NormalFactory(),
    ot.TriangularFactory(),
    ot.ExponentialFactory(),
    ot.GammaFactory(),
]
result, norms = distribution.project(collFactories)
assert len(result) == len(collFactories), "wrong number of projections"
assert len(norms) == len(collFactories), "wrong number of norms"
for i in range(len(norms)):
    assert norms[i] >= 0.0 and norms[i] < 1.0, "invalid Kolmogorov norm"

# ------------------------------ Multivariate tests ------------------------------#
# 2D LinearCombinationDistribution made of Normal atoms, compared to the exact
# Gaussian density
collection = [ot.Normal(0.0, 1.0)] * 3

weightMatrix = ot.Matrix(2, 3)
weightMatrix[0, 0] = 1.0
weightMatrix[0, 1] = -2.0
weightMatrix[0, 2] = 1.0
weightMatrix[1, 0] = 1.0
weightMatrix[1, 1] = 1.0
weightMatrix[1, 2] = -3.0

# Build the LinearCombinationDistribution
distribution2D = ot.LinearCombinationDistribution(collection, weightMatrix)
distribution2D.setBlockMin(3)
distribution2D.setBlockMax(10)

# Build a grid for validation
xMin = distribution2D.getRange().getLowerBound()[0]
xMax = distribution2D.getRange().getUpperBound()[0]
yMin = distribution2D.getRange().getLowerBound()[1]
yMax = distribution2D.getRange().getUpperBound()[1]
# Number of points of discretization
nx = 4
ny = 4
boxParameters = [nx, ny]
boxGrid = ot.Box(boxParameters)
grid = boxGrid.generate()
# scaling box grid
scaleFactor = [0.25 * (xMax - xMin), 0.25 * (yMax - yMin)]
grid *= scaleFactor
# translating
translateFactor = distribution2D.getMean()[0:2]
grid += translateFactor
# Compute PDF
# parameters for theoritical PDF, obtained thanks to Maple
factor = sqrt(2) / (20 * pi)
for index in range(grid.getSize()):
    point = grid[index]
    PDF = distribution2D.computePDF(point)
    # Very small values are not very accurate on x86, skip them
    if PDF < 1.0e-12:
        continue
    x, y = tuple(point)
    pdf_ref = factor * exp(-3.0 / 50.0 * y * y - 2.0 / 25 * x * y - 11.0 / 100 * x * x)
    ott.assert_almost_equal(PDF, pdf_ref, 1e-5, 1e-12, "2D gaussian pdf")

# 2D test made of uniform atoms
collUniforme = [ot.Uniform(0, 1)] * 3
# Build the LinearCombinationDistribution
dist_2D = ot.LinearCombinationDistribution(collUniforme, weightMatrix)
dist_2D.setBlockMin(3)
dist_2D.setBlockMax(8)

ott.assert_almost_equal(dist_2D.getMean(), [0.0, -0.5], 1e-12, 1e-12)
covarianceRef = ot.CovarianceMatrix(2)
covarianceRef[0, 0] = (1.0 + 4.0 + 1.0) / 12.0
covarianceRef[1, 1] = (1.0 + 1.0 + 9.0) / 12.0
covarianceRef[1, 0] = (1.0 - 2.0 - 3.0) / 12.0
ott.assert_almost_equal(dist_2D.getCovariance(), covarianceRef, 1e-12, 1e-12)

# Discretization on grid mu, mu + sigma
newGrid = boxGrid.generate()
# scaling box grid
newGrid *= dist_2D.getStandardDeviation()
# translating
newGrid += dist_2D.getMean()
# Compute PDF
for index in range(newGrid.getSize()):
    point = newGrid[index]
    PDF = dist_2D.computePDF(point)
    assert PDF >= 0.0 and PDF < float("inf"), "invalid pdf"

# 3D test
ot.ResourceMap.SetAsUnsignedInteger(
    "LinearCombinationDistribution-DefaultMaxSize", 8290688
)
mixture = ot.Mixture([ot.Normal(2, 1), ot.Normal(-2, 1)])
collection = [ot.Normal(0.0, 1.0), mixture, ot.Uniform(0, 1), ot.Uniform(0, 1)]
matrix = ot.Matrix([[1, -0.05, 1, -0.5], [0.5, 1, -0.05, 0.3], [-0.5, -0.1, 1.2, -0.8]])
dist_3D = ot.LinearCombinationDistribution(collection, matrix)
dist_3D.setBlockMin(3)
dist_3D.setBlockMax(6)

# Total number of points (is (2+2)**3)
# Test is CPU consuming
N = 2
b = ot.Box([N, N, N])
# Grid ==> (mu, mu+sigma)
grid3D = b.generate() * dist_3D.getStandardDeviation() + dist_3D.getMean()

for i in range(grid3D.getSize() // 4):
    point = grid3D[4 * i]
    PDF = dist_3D.computePDF(point)
    assert PDF >= 0.0 and PDF < float("inf"), "invalid pdf"

# For ticket 882
mixture = ot.LinearCombinationDistribution([ot.Dirac()])
graph = mixture.drawPDF()
graph = mixture.drawCDF()

# Test computeQuantile for the specific case of an analytical 1D mixture
case1 = 0.1 * ot.ChiSquare()
q = case1.computeQuantile(0.95)[0]
ott.assert_almost_equal(case1.computeCDF(q), 0.95, 1e-7, 1e-9, "case 1 quantile")
q = case1.computeQuantile(0.95, True)[0]
ott.assert_almost_equal(
    case1.computeComplementaryCDF(q), 0.95, 1e-7, 1e-9, "case 1 complementary quantile"
)
case2 = -0.1 * ot.ChiSquare()
q = case2.computeQuantile(0.95)[0]
ott.assert_almost_equal(case2.computeCDF(q), 0.95, 1e-7, 1e-9, "case 2 quantile")
q = case2.computeQuantile(0.95, True)[0]
ott.assert_almost_equal(
    case2.computeComplementaryCDF(q), 0.95, 1e-7, 1e-9, "case 2 complementary quantile"
)
# For ticket 953
atom1 = ot.TruncatedDistribution(ot.Uniform(0.0, 1.0), 0.0, 1.0)
atom2 = ot.Uniform(0.0, 2.0)
total = atom1 + atom2
ott.assert_almost_equal(
    total.computeCDF(2.0), 0.75, 1e-10, 1e-12, "ticket 953 cdf"
)
quantile = total.computeQuantile(0.2)[0]
ott.assert_almost_equal(
    quantile, sqrt(0.8), 1e-8, 1e-10, "ticket 953 quantile"
)
minS = 0.2
maxS = 10.0
muS = (log(minS) + log(maxS)) / 2.0
sigma = (log(maxS) - muS) / 3.0
atom1 = ot.TruncatedDistribution(ot.LogNormal(muS, sigma), minS, maxS)
atom2 = ot.Uniform(0.0, 2.0)
total = atom1 + atom2
cdfValue = total.computeCDF(2.0)
ott.assert_almost_equal(cdfValue, 0.306712, 1e-5, 1e-8, "ticket 953 bis cdf")
quantile = total.computeQuantile(0.2)[0]
ott.assert_almost_equal(
    total.computeCDF(quantile), 0.2, 1e-6, 1e-8, "ticket 953 bis quantile"
)
# For ticket 1129
dist = ot.LinearCombinationDistribution([ot.Uniform()] * 200)
ott.assert_almost_equal(dist.computeCDF([0]), 0.5, 1e-9, 1e-10, "ticket 1129")

# check parameter accessors
dist = ot.Gumbel() + ot.Normal(0, 0.1)
p = [1849.41, -133.6, -133.6, 359.172]
dist.setParameter(p)
ott.assert_almost_equal(dist.getParameter(), p, 0.0, 0.0, "parameter accessors")

# Test getPositionIndicator and getDispersionIndicator
print("position and dispersion indicators")
dist1d = ot.LinearCombinationDistribution(
    [ot.Normal(1.0, 2.0), ot.Uniform(-1.0, 1.0)], [1.0, 0.5]
)
pi = dist1d.getPositionIndicator()
ott.assert_almost_equal(pi, dist1d.getMean()[0], 1e-10, 1e-12, "position indicator")
di = dist1d.getDispersionIndicator()
ott.assert_almost_equal(di, dist1d.getStandardDeviation()[0], 1e-10, 1e-12, "dispersion indicator")

# Test setReferenceBandwidth / getReferenceBandwidth
print("reference bandwidth")
bw = dist1d.getReferenceBandwidth()
assert len(bw) == 1, "wrong bandwidth dimension"
newBw = [0.75]
dist1d.setReferenceBandwidth(newBw)
ott.assert_almost_equal(dist1d.getReferenceBandwidth(), newBw, 1e-12, 1e-12, "reference bandwidth")

# Test setPDFPrecision / setCDFPrecision
print("precision setters")
dist1d.setPDFPrecision(1.0e-12)
dist1d.setCDFPrecision(1.0e-12)

# Test setAlpha / setBeta
print("alpha and beta setters")
dist1d.setAlpha(8.0)
ott.assert_almost_equal(dist1d.getAlpha(), 8.0, 1e-12, 1e-12, "alpha setter")
dist1d.setBeta(10.0)
ott.assert_almost_equal(dist1d.getBeta(), 10.0, 1e-12, 1e-12, "beta setter")

# Test setMaxSize
print("max size setter")
dist1d.setMaxSize(1000000)
assert dist1d.getMaxSize() == 1000000, "wrong maxSize after setter"

# Test setBlockMin / setBlockMax
print("block min/max setters")
dist1d.setBlockMin(4)
assert dist1d.getBlockMin() == 4, "wrong blockMin after setter"
dist1d.setBlockMax(18)
assert dist1d.getBlockMax() == 18, "wrong blockMax after setter"
