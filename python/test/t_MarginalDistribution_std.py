#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dimension = 5
indices = [2, 0, 1]
coll = [ot.Normal(dimension), ot.Multinomial(10, [1.0 / (dimension + 2)] * dimension)]

for nDistribution in range(len(coll)):
    fullDistribution = coll[nDistribution]
    distribution = ot.MarginalDistribution(fullDistribution, indices)
    # To check if indices accessors are correct
    otherDistribution = distribution
    otherDistribution.setIndices(distribution.getIndices())
    print("Test indices accessors", otherDistribution == distribution)
    # To check if distribution accessors are correct
    otherDistribution.setDistribution(distribution.getDistribution())
    print("Test distribution accessors", otherDistribution == distribution)

    print("Distribution ", repr(distribution))
    print("Distribution ", distribution)

    # Is this distribution elliptical ?
    print("Elliptical = ", distribution.isElliptical())

    # Is this distribution continuous ?
    print("Continuous = ", distribution.isContinuous())

    # Is this distribution discrete ?
    print("Discrete = ", distribution.isDiscrete())

    # Is this distribution integral ?
    print("Integral = ", distribution.isIntegral())

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
    point = [1.0] * distribution.getDimension()
    print("Point= ", point)

    # Test the DDF/logDDF only in the continuous case
    if distribution.isContinuous():
        DDF = distribution.computeDDF(point)
        print("ddf     =", DDF)
        LPDF = distribution.computeLogPDF(point)
        print("log pdf=%.5e" % LPDF)

    # Show PDF and CDF of point
    PDF = distribution.computePDF(point)
    print("pdf     =%.5e" % PDF)
    CDF = distribution.computeCDF(point)
    print("cdf     =%.5e" % CDF)
    CCDF = distribution.computeComplementaryCDF(point)
    print("ccdf    =%.5e" % CCDF)
    Survival = distribution.computeSurvivalFunction(point)
    print("survival=%.5e" % Survival)
    if distribution.isContinuous():
        InverseSurvival = distribution.computeInverseSurvivalFunction(0.95)
        print("Inverse survival=", InverseSurvival)
        print(
            "Survival(inverse survival)=%.5e"
            % distribution.computeSurvivalFunction(InverseSurvival)
        )
    quantile = distribution.computeQuantile(0.95)
    print("quantile=", quantile)
    print("cdf(quantile)=%.5e" % distribution.computeCDF(quantile))
    quantileTail = distribution.computeQuantile(0.95, True)
    print("quantile (tail)=", quantileTail)
    CDFTail = distribution.computeComplementaryCDF(quantileTail)
    print("cdf (tail)=%.5e" % CDFTail)
    mean = distribution.getMean()
    print("mean=", mean)
    standardDeviation = distribution.getStandardDeviation()
    print("standard deviation=", standardDeviation)
    skewness = distribution.getSkewness()
    print("skewness=", skewness)
    kurtosis = distribution.getKurtosis()
    print("kurtosis=", kurtosis)
    covariance = distribution.getCovariance()
    print("covariance=", covariance)
    correlation = distribution.getCorrelation()
    print("correlation=", correlation)
    if distribution.isContinuous():
        spearman = distribution.getSpearmanCorrelation()
        print("spearman=", spearman)
        kendall = distribution.getKendallTau()
        print("kendall=", kendall)
    print(
        "Standard representative=", distribution.getStandardRepresentative().__str__()
    )
    ot.Log.Show(ot.Log.TRACE)
    validation = ott.DistributionValidation(distribution)
    validation.skipDependenceMeasures()  # slow
    validation.skipConditional()  # TRACE enabled, avoid LOGTRACE in output
    validation.skipTransformation()  # FIXME
    validation.run()

# Test computeSequentialConditionalQuantile wrapping BlockIndependentCopula
marginals = [ot.Normal()] * 4
a = 0.8
b = 0.1
copulas = [
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, a], [a, 1.0]])),
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, b], [b, 1.0]]))
]
block_independent_copula = ot.BlockIndependentCopula(copulas)
point = ot.Point([0.7] * 4)

# Direct BlockIndependentCopula (contiguous blocks)
joint_direct = ot.JointDistribution(marginals, block_independent_copula)
ref = joint_direct.computeSequentialConditionalQuantile(point)

# MarginalDistribution with reordering (non-contiguous blocks)
marginal_distribution = block_independent_copula.getMarginal([0, 2, 1, 3])
joint_permuted = ot.JointDistribution(marginals, marginal_distribution)
result = joint_permuted.computeSequentialConditionalQuantile(point)

# Expected: result is a permutation of ref
# ref for BIC [NormalCopula(a), NormalCopula(b)] gives SCQ components in order 0,1,2,3
# With indices [0,2,1,3], result should be [ref[0], ref[2], ref[1], ref[3]]
expected = [ref[0], ref[2], ref[1], ref[3]]
ott.assert_almost_equal(result, expected, 1e-5, 1e-5)

# Test computeConditionalCDF and computeConditionalQuantile on MarginalDistribution
# Case 1: Independent normal components
fullNormal = ot.Normal(3)
margNormal = ot.MarginalDistribution(fullNormal, [2, 0, 1])
x = 0.5

# conditioning dimension 0 => should match the marginal CDF
cdf0 = margNormal.computeConditionalCDF(x, [])
ott.assert_almost_equal(cdf0, ot.Normal().computeCDF(x))

# conditioning dimension 1 with independent copula => same as unconditional
cdf1 = margNormal.computeConditionalCDF(x, [0.3])
ott.assert_almost_equal(cdf1, ot.Normal().computeCDF(x))

# conditional quantile with conditioning dimension 0
q = 0.95
q0 = margNormal.computeConditionalQuantile(q, [])
ott.assert_almost_equal(q0, ot.Normal().computeQuantile(q)[0])

# conditional quantile with conditioning dimension 1 (independent)
q1 = margNormal.computeConditionalQuantile(q, [0.3])
ott.assert_almost_equal(q1, ot.Normal().computeQuantile(q)[0])

# Case 2: Dependent normal components (via NormalCopula)
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.8
fullDep = ot.Normal([0.0] * 3, [1.0] * 3, R)
margDep = ot.MarginalDistribution(fullDep, [0, 1])
# Conditional distribution of X1 | X0 = 0.3 is Normal(0.3*0.8, sqrt(1-0.8^2))
rho = 0.8
condDist = ot.Normal(0.3 * rho, (1.0 - rho ** 2) ** 0.5)
cdfCond = margDep.computeConditionalCDF(0.5, [0.3])
ott.assert_almost_equal(cdfCond, condDist.computeCDF(0.5))
qCond = margDep.computeConditionalQuantile(0.95, [0.3])
ott.assert_almost_equal(qCond, condDist.computeQuantile(0.95)[0])

# Case 3: consistency between conditional CDF and conditional quantile
qTest = 0.3
xFromQ = margDep.computeConditionalQuantile(qTest, [0.3])
cdfFromQ = margDep.computeConditionalCDF(xFromQ, [0.3])
ott.assert_almost_equal(cdfFromQ, qTest)
