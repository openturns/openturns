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

# Test SCQ on MarginalDistribution wrapping a JointDistribution
# whose core is a BlockIndependentCopula (not the copula directly)
marginals_jd = [
    ot.Normal(0.0, 1.0), ot.Normal(1.0, 2.0),
    ot.Exponential(1.0), ot.Uniform(-1.0, 2.0),
]
copulas_jd = [
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, a], [a, 1.0]])),
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, b], [b, 1.0]])),
]
bic_jd = ot.BlockIndependentCopula(copulas_jd)
joint_direct = ot.JointDistribution(marginals_jd, bic_jd)
perm = [0, 2, 1, 3]
marg_dist_jd = ot.MarginalDistribution(joint_direct, perm)

# Use distinct q values so the permutation of q is also tested
point_jd = ot.Point([0.7, 0.3, 0.5, 0.9])
result_jd = marg_dist_jd.computeSequentialConditionalQuantile(point_jd)
# Reference: permute q to global order, compute direct, permute result back
q_global = ot.Point([point_jd[perm.index(i)] for i in range(4)])
ref_jd = joint_direct.computeSequentialConditionalQuantile(q_global)
expected_jd = [ref_jd[perm[i]] for i in range(4)]
ott.assert_almost_equal(result_jd, expected_jd, 1e-5, 1e-5)

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

# Test computeSequentialConditionalQuantile with custom ordering on BlockIndependentCopula
a = 0.8
b = 0.1
copulas_bic = [
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, a], [a, 1.0]])),
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, b], [b, 1.0]])),
]
bic = ot.BlockIndependentCopula(copulas_bic)
point_bic = ot.Point([0.3, 0.7, 0.5, 0.9])
# Identity ordering should match natural order
ott.assert_almost_equal(
    bic.computeSequentialConditionalQuantile(point_bic, [0, 1, 2, 3]),
    bic.computeSequentialConditionalQuantile(point_bic))
# Partial ordering on BIC: first block (dim=2)
# Compare against the block copula directly
block0 = copulas_bic[0]
ott.assert_almost_equal(
    bic.computeSequentialConditionalQuantile(ot.Point([0.3, 0.7]), [0, 1]),
    block0.computeSequentialConditionalQuantile(ot.Point([0.3, 0.7])), 1e-5, 1e-5)
# Partial ordering from the second block
block1 = copulas_bic[1]
ott.assert_almost_equal(
    bic.computeSequentialConditionalQuantile(ot.Point([0.5, 0.9]), [2, 3]),
    block1.computeSequentialConditionalQuantile(ot.Point([0.5, 0.9])), 1e-5, 1e-5)

# Test computeSequentialConditionalQuantile with custom ordering on JointDistribution
marginals_jd2 = [
    ot.Normal(0.0, 1.0), ot.Normal(1.0, 2.0),
    ot.Exponential(1.0), ot.Uniform(-1.0, 2.0),
]
joint_direct2 = ot.JointDistribution(marginals_jd2, bic)
# Identity ordering
ott.assert_almost_equal(
    joint_direct2.computeSequentialConditionalQuantile(point_bic, [0, 1, 2, 3]),
    joint_direct2.computeSequentialConditionalQuantile(point_bic))
# Non-trivial permutation: [2, 0, 3, 1]
perm2 = [2, 0, 3, 1]
ref_jd2 = ot.MarginalDistribution(joint_direct2, perm2).computeSequentialConditionalQuantile(point_bic)
ott.assert_almost_equal(
    joint_direct2.computeSequentialConditionalQuantile(point_bic, perm2),
    ref_jd2, 1e-4, 1e-4)

# Test on a generic distribution (Normal) via the default implementation
normal4 = ot.Normal([0.0] * 4, [1.0] * 4, ot.CorrelationMatrix(4))
ott.assert_almost_equal(
    normal4.computeSequentialConditionalQuantile(point_bic, [0, 1, 2, 3]),
    normal4.computeSequentialConditionalQuantile(point_bic))
# Permutation on generic Normal
ref_norm2 = ot.MarginalDistribution(normal4, [0, 2, 1, 3]).computeSequentialConditionalQuantile(point_bic)
ott.assert_almost_equal(
    normal4.computeSequentialConditionalQuantile(point_bic, [0, 2, 1, 3]),
    ref_norm2, 1e-4, 1e-4)
# Partial ordering
ott.assert_almost_equal(
    normal4.computeSequentialConditionalQuantile(ot.Point([0.3, 0.7]), [0, 2]),
    ot.MarginalDistribution(normal4, [0, 2]).computeSequentialConditionalQuantile(ot.Point([0.3, 0.7])),
    1e-4, 1e-4)

# Test computeSequentialConditionalCDF wrapping BlockIndependentCopula
# with permutation that preserves block-internal order
a = 0.8
b = 0.1
copulas_sc = [
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, a], [a, 1.0]])),
    ot.NormalCopula(ot.CorrelationMatrix([[1.0, b], [b, 1.0]])),
]
bic_cdf = ot.BlockIndependentCopula(copulas_sc)

# Use distinct x values so the permutation is tested
perm_cdf = [0, 2, 1, 3]
x_global = ot.Point([0.7, 0.4, 0.6, 0.8])

# Direct BlockIndependentCopula (contiguous blocks)
ref_cdf = bic_cdf.computeSequentialConditionalCDF(x_global)

# MarginalDistribution with reordering (non-contiguous blocks)
marg_dist_cdf = bic_cdf.getMarginal(perm_cdf)

# Permute x to match the marginal order
x_marg = ot.Point([x_global[perm_cdf[i]] for i in range(4)])
result_cdf = marg_dist_cdf.computeSequentialConditionalCDF(x_marg)

# Expected: result is the inverse permutation of ref
expected_cdf = [ref_cdf[perm_cdf[i]] for i in range(4)]
ott.assert_almost_equal(result_cdf, expected_cdf, 1e-5, 1e-5)

# Test computeSequentialConditionalPDF wrapping BlockIndependentCopula
x_global_pdf = ot.Point([0.7, 0.4, 0.6, 0.8])
ref_pdf = bic_cdf.computeSequentialConditionalPDF(x_global_pdf)
x_marg_pdf = ot.Point([x_global_pdf[perm_cdf[i]] for i in range(4)])
result_pdf = marg_dist_cdf.computeSequentialConditionalPDF(x_marg_pdf)
expected_pdf = [ref_pdf[perm_cdf[i]] for i in range(4)]
ott.assert_almost_equal(result_pdf, expected_pdf, 1e-5, 1e-5)
