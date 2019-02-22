#! /usr/bin/env python

from __future__ import print_function
from openturns import *
from math import sqrt, pi, exp, log

TESTPREAMBLE()
RandomGenerator.SetSeed(0)
ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 4000000)
# Deactivate the simplification mechanism as we want to test the Poisson formula
# based algorithm here
ResourceMap.SetAsBool("RandomMixture-SimplifyAtoms", False)

try:
    # Create a collection of test-cases and the associated references
    numberOfTests = 3
    testCases = list()
    references = DistributionCollection(numberOfTests)
    testCases.append(DistributionCollection(2))
    testCases[0][0] = Uniform(-1.0, 3.0)
    testCases[0][1] = Uniform(-1.0, 3.0)
    references[0] = Triangular(-2.0, 2.0, 6.0)
    testCases.append(DistributionCollection(3))
    testCases[1][0] = Normal()
    testCases[1][1] = Normal(1.0, 2.0)
    testCases[1][2] = Normal(-2.0, 2.0)
    references[1] = Normal(-1.0, 3.0)
    testCases.append(DistributionCollection(3))
    testCases[2][0] = Exponential()
    testCases[2][1] = Exponential()
    testCases[2][2] = Exponential()
    references[2] = Gamma(3.0, 1.0, 0.0)
    print("testCases=", testCases)
    print("references=", references)
    for testIndex in range(len(testCases)):
        # Instanciate one distribution object
        distribution = RandomMixture(testCases[testIndex])
        distribution.setBlockMin(5)
        distribution.setBlockMax(20)
        distributionReference = references[testIndex]
        print("Distribution ", repr(distribution))
        print("Distribution ", distribution)

        # Is this distribution elliptical ?
        print("Elliptical = ", distribution.isElliptical())

        # Is this distribution continuous ?
        print("Continuous = ", distribution.isContinuous())

        # Test for realization of distribution
        oneRealization = distribution.getRealization()
        print("oneRealization=", oneRealization)

        # Test for sampling
        size = 10000
        oneSample = distribution.getSample(size)
        print("oneSample first=", oneSample[0], " last=", oneSample[size - 1])
        print("mean=", oneSample.computeMean())
        print("covariance=", oneSample.computeCovariance())

        # Define a point
        point = Point(distribution.getDimension(), 0.5)
        print("Point= ", point)

        # Show PDF and CDF of point
        eps = 1e-5
        DDF = distribution.computeDDF(point)[0]
        print("ddf      =%.5g" % DDF)
        print("ddf (ref)=", distributionReference.computeDDF(point))
        PDF = distribution.computePDF(point)
        print("pdf      =%.6f" % PDF)
        print("pdf  (FD)=%.6f" % ((distribution.computeCDF(point + Point(1, eps)) -
                                   distribution.computeCDF(point + Point(1, -eps))) / (2.0 * eps)))
        print("pdf (ref)=%.6f" % distributionReference.computePDF(point))
        CDF = distribution.computeCDF(point)
        print("cdf      =%.6f" % CDF)
        print("cdf (ref)=%.6f" % distributionReference.computeCDF(point))
        CF = distribution.computeCharacteristicFunction(point[0])
        print("characteristic function=%.6f + %.6fi" % (CF.real, CF.imag))
        LCF = distribution.computeLogCharacteristicFunction(point[0])
        print("log characteristic function=%.6f + %.6fi" %
              (LCF.real, LCF.imag))
        quantile = distribution.computeQuantile(0.95)
        print("quantile      =", quantile)
        print("quantile (ref)=", distributionReference.computeQuantile(0.95))
        print("cdf(quantile)=%.6f" % distribution.computeCDF(quantile))
        quantileComp = distribution.computeQuantile(0.95, True)
        print("quantile comp.=", quantileComp)
        print("cdfComp(quantileComp)=%.6f" %
              distribution.computeComplementaryCDF(quantileComp))
        # Get 95% survival function
        inverseSurvival = Point(
            distribution.computeInverseSurvivalFunction(0.95))
        print("InverseSurvival=", repr(inverseSurvival))
        print("Survival(inverseSurvival)=%.6f" %
              distribution.computeSurvivalFunction(inverseSurvival))
        # Entropy: too expansive for now...
        if False:
            print("entropy=%.6f" % distribution.computeEntropy())

        # Confidence regions: too expansive for now...
        if False:
            print("dimension=", distribution.getDimension(),
                  "test case=", testIndex)
            interval, threshold = distribution.computeMinimumVolumeIntervalWithMarginalProbability(
                0.95)
            print("Minimum volume interval=", interval)
            print("threshold=", Point(1, threshold))
            levelSet, beta = distribution.computeMinimumVolumeLevelSetWithThreshold(
                0.95)
            print("Minimum volume level set=", levelSet)
            print("beta=", Point(1, beta))
            interval, beta = distribution.computeBilateralConfidenceIntervalWithMarginalProbability(
                0.95)
            print("Bilateral confidence interval=", interval)
            print("beta=", Point(1, beta))
            interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
                0.95, False)
            print("Unilateral confidence interval (lower tail)=", interval)
            print("beta=", Point(1, beta))
            interval, beta = distribution.computeUnilateralConfidenceIntervalWithMarginalProbability(
                0.95, True)
            print("Unilateral confidence interval (upper tail)=", interval)
            print("beta=", Point(1, beta))

        mean = distribution.getMean()
        print("mean      =", mean)
        print("mean (ref)=", distributionReference.getMean())
        standardDeviation = distribution.getStandardDeviation()
        print("standard deviation      =", standardDeviation)
        print("standard deviation (ref)=",
              distributionReference.getStandardDeviation())
        skewness = distribution.getSkewness()
        print("skewness      =", skewness)
        print("skewness (ref)=", distributionReference.getSkewness())
        kurtosis = distribution.getKurtosis()
        print("kurtosis      =", kurtosis)
        print("kurtosis (ref)=", distributionReference.getKurtosis())
        covariance = distribution.getCovariance()
        print("covariance      =", covariance)
        print("covariance (ref)=", distributionReference.getCovariance())
        parameters = distribution.getParametersCollection()
        print("parameters=", parameters)
        print("Standard representative=",
              distribution.getStandardRepresentative())
        print("blockMin=", distribution.getBlockMin())
        print("blockMax=", distribution.getBlockMax())
        print("maxSize=", distribution.getMaxSize())
        print("alpha=", distribution.getAlpha())
        print("beta=", distribution.getBeta())
    # Tests of the simplification mechanism
    weights = Point(0)
    coll = DistributionCollection(0)
    coll.add(Dirac(0.5))
    weights.add(1.0)
    coll.add(Normal(1.0, 2.0))
    weights.add(2.0)
    coll.add(Normal(2.0, 1.0))
    weights.add(-3.0)
    coll.add(Uniform(-2.0, 2.0))
    weights.add(-1.0)
    coll.add(Uniform(2.0, 4.0))
    weights.add(2.0)
    coll.add(Exponential(2.0, -3.0))
    weights.add(1.5)
    rm = RandomMixture(coll, weights)
    coll.add(rm)
    weights.add(-2.5)
    coll.add(Gamma(3.0, 4.0, -2.0))
    weights.add(2.5)
    distribution = RandomMixture(coll, weights)
    print("distribution=", repr(distribution))
    print("distribution=", distribution)
    mu = distribution.getMean()[0]
    sigma = distribution.getStandardDeviation()[0]
    for i in range(10):
        x = mu + (-3.0 + 6.0 * i / 9.0) * sigma
        print("pdf( %.6f )=%.6f" % (x, distribution.computePDF(x)))

    # Tests of the projection mechanism
    collFactories = [UniformFactory(), NormalFactory(
    ), TriangularFactory(), ExponentialFactory(), GammaFactory()]
    #, TrapezoidalFactory()
    result, norms = distribution.project(collFactories)
    print("projections=", result)
    print("norms=", norms)
    # ------------------------------ Multivariate tests ------------------------------#
    # 2D RandomMixture
    collection = DistributionCollection(0)
    collection.add(Normal(0.0, 1.0))
    collection.add(Normal(0.0, 1.0))
    collection.add(Normal(0.0, 1.0))

    weightMatrix = Matrix(2, 3)
    weightMatrix[0, 0] = 1.0
    weightMatrix[0, 1] = -2.0
    weightMatrix[0, 2] = 1.0
    weightMatrix[1, 0] = 1.0
    weightMatrix[1, 1] = 1.0
    weightMatrix[1, 2] = -3.0

    # Build the RandomMixture
    distribution2D = RandomMixture(collection, weightMatrix)
    print("distribution = ", distribution2D)
    print("range = ", distribution2D.getRange())
    print("mean = ",  distribution2D.getMean())
    print("cov = ", distribution2D.getCovariance())
    print("sigma = ", distribution2D.getStandardDeviation())
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
    boxParameters = Point(2)
    boxParameters[0] = nx
    boxParameters[1] = ny
    boxGrid = Box(boxParameters)
    grid = boxGrid.generate()
    # scaling box grid
    scaleFactor = Point(2)
    scaleFactor[0] = 0.25 * (xMax - xMin)
    scaleFactor[1] = 0.25 * (yMax - yMin)
    grid *= scaleFactor
    # translating
    translateFactor = Point(2)
    translateFactor[0] = distribution2D.getMean()[0]
    translateFactor[1] = distribution2D.getMean()[1]
    grid += translateFactor
    # Compute PDF
    # parameters for theoritical PDF, obtained thanks to Maple
    factor = sqrt(2) / (20 * pi)
    for index in range(grid.getSize()):
        point = grid[index]
        PDF = distribution2D.computePDF(point)
        # Very small values are not very accurate on x86, skip them
        if (PDF < 1.e-12):
            continue
        print("pdf      = %.6g" % PDF)
        x, y = tuple(point)
        pdf_ref = factor * \
            exp(-3.0 / 50.0 * y * y - 2.0 / 25 * x * y - 11.0 / 100 * x * x)
        print("pdf (ref)= %.6g" % pdf_ref)

    # 2D test, but too much CPU consuming
    collUniforme = DistributionCollection(0)
    collUniforme.add(Uniform(0, 1))
    collUniforme.add(Uniform(0, 1))
    collUniforme.add(Uniform(0, 1))
    # Build the RandomMixture
    dist_2D = RandomMixture(collUniforme, weightMatrix)
    dist_2D.setBlockMin(3)
    dist_2D.setBlockMax(8)

    print("new distribution = ", dist_2D)
    print("range = ", dist_2D.getRange())
    print("mean = ",  dist_2D.getMean())
    print("cov = ", dist_2D.getCovariance())
    print("sigma = ", dist_2D.getStandardDeviation())

    # Discretization on grid mu, mu + \sigma
    newGrid = boxGrid.generate()
    # scaling box grid
    newGrid *= dist_2D.getStandardDeviation()
    # translating
    newGrid += dist_2D.getMean()
    # Compute PDF
    for index in range(newGrid.getSize()):
        point = newGrid[index]
        PDF = dist_2D.computePDF(point)
        print("pdf      = %.6g" % PDF)

    # 3D test
    ResourceMap.SetAsUnsignedInteger("RandomMixture-DefaultMaxSize", 8290688)
    mixture = Mixture([Normal(2, 1), Normal(-2, 1)])
    collection = [Normal(0.0, 1.0), mixture, Uniform(0, 1), Uniform(0, 1)]
    matrix = Matrix(
        [[1, -0.05, 1, -0.5], [0.5, 1, -0.05, 0.3], [-0.5, -0.1, 1.2, -0.8]])
    dist_3D = RandomMixture(collection, matrix)
    dist_3D.setBlockMin(3)
    dist_3D.setBlockMax(6)

    print("3D distribution = ", dist_3D)
    print("range = ", dist_3D.getRange())
    print("mean = ",  dist_3D.getMean())
    print("cov = ", dist_3D.getCovariance())
    print("sigma = ", dist_3D.getStandardDeviation())
    # Total number of points (is (2+2)**3)
    # Test is CPU consuming
    N = 2
    b = Box([N, N, N])
    # Grid ==> (mu, mu+sigma)
    grid3D = b.generate() * dist_3D.getStandardDeviation() + dist_3D.getMean()

    for i in range(grid3D.getSize() // 4):
        point = grid3D[4 * i]
        PDF = dist_3D.computePDF(point)
        print("pdf      = %.6g" % PDF)

    # For ticket 882
    mixture = RandomMixture([Dirac()])
    graph = mixture.drawPDF()
    graph = mixture.drawCDF()

    # Test computeQuantile for the specific case of an analytical 1D mixture
    case1 = 0.1 * ChiSquare()
    q = case1.computeQuantile(0.95)[0]
    print("case 1, q=%.6f" % q)
    q = case1.computeQuantile(0.95, True)[0]
    print("case 1, q comp=%.6f" % q)
    case2 = -0.1 * ChiSquare()
    q = case2.computeQuantile(0.95)[0]
    print("case 2, q=%.6f" % q)
    q = case2.computeQuantile(0.95, True)[0]
    print("case 2, q comp=%.6f" % q)
    # For ticket 953
    atom1 = TruncatedDistribution(Uniform(0.0, 1.0), 0.0, 1.0)
    atom2 = Uniform(0.0, 2.0)
    sum = atom1 + atom2
    print("sum=", sum)
    print("CDF=%.6g" % sum.computeCDF(2.0))
    print("quantile=", sum.computeQuantile(0.2))
    minS = 0.2
    maxS = 10.0
    muS = (log(minS) + log(maxS)) / 2.0
    sigma = (log(maxS) - muS) / 3.0
    atom1 = TruncatedDistribution(LogNormal(muS, sigma), minS, maxS)
    atom2 = Uniform(0.0, 2.0)
    sum = atom1 + atom2
    print("sum=", sum)
    print("CDF=%.6g" % sum.computeCDF(2.0))
    print("quantile=", sum.computeQuantile(0.2))
except:
    import sys
    print("t_RandomMixture_std.py", sys.exc_info()[0], sys.exc_info()[1])
