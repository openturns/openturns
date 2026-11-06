#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Instantiate one distribution object
R = ot.CorrelationMatrix(3)
R[0, 1] = 0.5
R[0, 2] = 0.25
collection = [ot.FrankCopula(3.0), ot.NormalCopula(R), ot.ClaytonCopula(2.0)]
copula = ot.BlockIndependentCopula(collection)

assert copula.getClassName() == "BlockIndependentCopula"
assert copula.getDimension() == 7
assert len(copula.getCopulaCollection()) == 3

# Is this copula elliptical ?
assert not copula.isElliptical()

# Is this copula continuous ?
assert copula.isContinuous()

# Is this copula elliptical ?
assert not copula.hasEllipticalCopula()

# Is this copula independent ?
assert not copula.hasIndependentCopula()

# Test for realization of copula
oneRealization = copula.getRealization()
ott.assert_almost_equal(
    oneRealization,
    [0.62987656, 0.89375790, 0.10272556, 0.15561710, 0.81617192, 0.06320608, 0.05604015],
    1e-6,
    0.0,
)

# Test for sampling: empirical moments match the theoretical ones
size = 10000
oneSample = copula.getSample(size)
assert oneSample.getSize() == size
assert oneSample.getDimension() == copula.getDimension()
ott.assert_almost_equal(oneSample.computeMean(), [0.5] * 7, 0.0, 2e-2)
ott.assert_almost_equal(
    oneSample.computeCovariance(), copula.getCovariance(), 2e-1, 5e-3
)

# Define a point
point = ot.Point(copula.getDimension(), 0.6)
ott.assert_almost_equal(point, [0.6] * 7, 0.0, 0.0)

# Show PDF and CDF of point
DDF = copula.computeDDF(point)
ott.assert_almost_equal(
    DDF,
    [0.58090522, 0.58090522, 0.91183031, 0.26052294, 0.13026147, 0.17764792, 0.17764792],
    1e-6,
    1e-8,
)
PDF = copula.computePDF(point)
ott.assert_almost_equal(PDF, 2.185069, 1e-6, 1e-6)
CDF = copula.computeCDF(point)
ott.assert_almost_equal(CDF, 0.058797, 1e-6, 1e-6)
quantile = copula.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [0.992521] * 7, 1e-6, 1e-8)
ott.assert_almost_equal(copula.computeCDF(quantile), 0.95, 1e-6, 1e-6)
# Get 95% survival function
inverseSurvival = ot.Point(copula.computeInverseSurvivalFunction(0.95))
ott.assert_almost_equal(inverseSurvival, [0.00832738] * 7, 1e-6, 1e-8)
ott.assert_almost_equal(
    copula.computeSurvivalFunction(inverseSurvival), 0.95, 1e-6, 1e-6
)
ott.assert_almost_equal(copula.computeEntropy(), -0.729844, 1e-6, 1e-6)

mean = copula.getMean()
ott.assert_almost_equal(mean, [0.5] * 7, 0.0, 0.0)
covariance = copula.getCovariance()
covarianceRef = ot.CovarianceMatrix(7)
for i in range(7):
    covarianceRef[i, i] = 1.0 / 12.0
covarianceRef[0, 1] = 0.037392913678273534
covarianceRef[2, 3] = 0.04021531162758312
covarianceRef[2, 4] = 0.019946543837384114
covarianceRef[5, 6] = 0.056852819440030566
ott.assert_almost_equal(covariance, covarianceRef, 1e-8, 1e-8)
parameters = copula.getParametersCollection()
ott.assert_almost_equal(parameters[0], [3.0, 0.5, 0.25, 0.0, 2.0], 1e-12, 0.0)
# Covariance and correlation
correlation = copula.getCorrelation()
correlationRef = ot.CorrelationMatrix(7)
correlationRef[0, 1] = 0.4487149641392824
correlationRef[2, 3] = 0.4825837395309975
correlationRef[2, 4] = 0.23935852604860938
correlationRef[5, 6] = 0.6822338332803668
ott.assert_almost_equal(correlation, correlationRef, 1e-8, 1e-8)
spearman = copula.getSpearmanCorrelation()
ott.assert_almost_equal(spearman, correlation, 1e-12, 1e-12)
kendall = copula.getKendallTau()
kendallRef = ot.CorrelationMatrix(7)
kendallRef[0, 1] = 0.3072469594307238
kendallRef[2, 3] = 0.33333333333333337
kendallRef[2, 4] = 0.16086124651033248
kendallRef[5, 6] = 0.5
ott.assert_almost_equal(kendall, kendallRef, 1e-8, 1e-8)
dim = copula.getDimension()
x = 0.6
y = [0.2] * (dim - 1)
ott.assert_almost_equal(copula.computeConditionalPDF(x, y), 0.467887, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalCDF(x, y), 0.902087, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalQuantile(x, y), 0.299570, 1e-6, 1e-6)
pt = ot.Point([0.1 * i + 0.05 for i in range(dim)])
ott.assert_almost_equal(
    copula.computeSequentialConditionalPDF(point),
    [1.0, 1.21219007, 1.0, 1.17967143, 1.05262354, 1.0, 1.45164487],
    1e-6,
    1e-8,
)
resCDF = copula.computeSequentialConditionalCDF(pt)
ott.assert_almost_equal(
    resCDF,
    [0.05, 0.34665263, 0.25, 0.47786492, 0.51455916, 0.55, 0.59506881],
    1e-6,
    1e-8,
)
ott.assert_almost_equal(
    copula.computeSequentialConditionalQuantile(resCDF), pt, 1e-8, 1e-8
)

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(copula)
validation.skipGradient()
validation.run()

# Specific to this copula

# Extract a 5-D marginal
dim = 5
point = ot.Point(dim, 0.25)
indices = ot.Indices(dim, 0)
indices[0] = 1
indices[1] = 2
indices[2] = 3
indices[3] = 5
indices[4] = 6
assert list(indices) == [1, 2, 3, 5, 6]
margins = copula.getMarginal(indices)
assert margins.getDimension() == 5
ott.assert_almost_equal(margins.computePDF(point), 3.086065, 1e-6, 1e-6)
ott.assert_almost_equal(margins.computeCDF(point), 0.005401, 1e-6, 1e-6)
quantile = margins.computeQuantile(0.95)
ott.assert_almost_equal(quantile, [0.989493] * 5, 1e-6, 1e-8)
ott.assert_almost_equal(margins.computeCDF(quantile), 0.95, 1e-6, 1e-6)
marginsRealization = margins.getRealization()
assert marginsRealization.getDimension() == 5
assert all(0.0 <= v <= 1.0 for v in marginsRealization)
# Tests the isoprobabilistic transformation
# General case with normal standard distribution
iso = copula.getIsoProbabilisticTransformation()
assert iso.getInputDimension() == 7
assert iso.getOutputDimension() == 7
isoInverse = copula.getInverseIsoProbabilisticTransformation()
isoPoint = ot.Point([0.1 * i + 0.05 for i in range(7)])
isoImage = iso(isoPoint)
ott.assert_almost_equal(
    isoImage,
    [
        -1.64485363,
        -0.39437357,
        -0.67448975,
        -0.05551291,
        0.03650251,
        0.12566135,
        0.24060358,
    ],
    1e-6,
    1e-8,
)
ott.assert_almost_equal(isoInverse(isoImage), isoPoint, 1e-8, 1e-8)
# General case with non-normal standard distribution
collection[0] = ot.SklarCopula(
    ot.Student(3.0, ot.Point(2, 1.0), ot.Point(2, 3.0), ot.CorrelationMatrix(2))
)
copula = ot.BlockIndependentCopula(collection)
iso = copula.getIsoProbabilisticTransformation()
assert iso.getInputDimension() == 7
assert iso.getOutputDimension() == 7
isoInverse = copula.getInverseIsoProbabilisticTransformation()
isoImage = iso(isoPoint)
ott.assert_almost_equal(
    isoImage,
    [
        -1.64485363,
        -0.77129059,
        -0.67448975,
        -0.05551291,
        0.03650251,
        0.12566135,
        0.24060358,
    ],
    1e-6,
    1e-8,
)
ott.assert_almost_equal(isoInverse(isoImage), isoPoint, 1e-8, 1e-8)
dim = copula.getDimension()
x = 0.6
y = [0.2] * (dim - 1)
ott.assert_almost_equal(copula.computeConditionalPDF(x, y), 0.467887, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalCDF(x, y), 0.902087, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalQuantile(x, y), 0.299570, 1e-6, 1e-6)
pt = ot.Point([0.1 * i + 0.05 for i in range(dim)])
ott.assert_almost_equal(
    copula.computeSequentialConditionalPDF(pt),
    [1.0, 1.06099249, 1.0, 1.24176743, 1.05204392, 1.0, 1.39118604],
    1e-6,
    1e-8,
)
resCDF = copula.computeSequentialConditionalCDF(pt)
ott.assert_almost_equal(
    resCDF,
    [0.05, 0.22026735, 0.25, 0.47786492, 0.51455916, 0.55, 0.59506881],
    1e-6,
    1e-8,
)
ott.assert_almost_equal(
    copula.computeSequentialConditionalQuantile(resCDF), pt, 1e-8, 1e-8
)
# Special case, independent copula
collection[0] = ot.SklarCopula(ot.Normal(2))
collection[1] = ot.IndependentCopula(2)
collection[2] = ot.NormalCopula(ot.CorrelationMatrix(2))
copula = ot.BlockIndependentCopula(collection)
iso = copula.getIsoProbabilisticTransformation()
assert iso.getInputDimension() == 6
assert iso.getOutputDimension() == 6
isoInverse = copula.getInverseIsoProbabilisticTransformation()
isoPoint = ot.Point([0.1 * i + 0.05 for i in range(6)])
isoImage = iso(isoPoint)
ott.assert_almost_equal(
    isoImage,
    [-1.64485363, -1.03643339, -0.67448975, -0.38532047, -0.12566135, 0.12566135],
    1e-6,
    1e-8,
)
ott.assert_almost_equal(isoInverse(isoImage), isoPoint, 1e-8, 1e-8)
dim = copula.getDimension()
x = 0.6
y = [0.2] * (dim - 1)
ott.assert_almost_equal(copula.computeConditionalPDF(x, y), 1.0, 1e-12, 1e-12)
ott.assert_almost_equal(copula.computeConditionalCDF(x, y), 0.6, 1e-12, 1e-12)
ott.assert_almost_equal(copula.computeConditionalQuantile(x, y), 0.6, 1e-12, 1e-12)
pt = ot.Point([0.1 * i + 0.05 for i in range(dim)])
ott.assert_almost_equal(
    copula.computeSequentialConditionalPDF(pt), [1.0] * 6, 1e-12, 1e-12
)
resCDF = copula.computeSequentialConditionalCDF(pt)
ott.assert_almost_equal(resCDF, pt, 1e-12, 1e-12)
ott.assert_almost_equal(
    copula.computeSequentialConditionalQuantile(resCDF), pt, 1e-12, 1e-12
)
# Special case, single contributor
collection = [
    ot.SklarCopula(
        ot.Student(3.0, ot.Point(2, 1.0), ot.Point(2, 3.0), ot.CorrelationMatrix(2))
    )
]
copula = ot.BlockIndependentCopula(collection)
iso = copula.getIsoProbabilisticTransformation()
assert iso.getInputDimension() == 2
assert iso.getOutputDimension() == 2
isoInverse = copula.getInverseIsoProbabilisticTransformation()
isoPoint = ot.Point([0.05, 0.15])
isoImage = iso(isoPoint)
ott.assert_almost_equal(isoImage, [-2.35336343, -1.24977811], 1e-6, 1e-8)
ott.assert_almost_equal(isoInverse(isoImage), isoPoint, 1e-8, 1e-8)
dim = copula.getDimension()
x = 0.6
y = [0.2] * (dim - 1)
ott.assert_almost_equal(copula.computeConditionalPDF(x, y), 1.028301, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalCDF(x, y), 0.602660, 1e-6, 1e-6)
ott.assert_almost_equal(copula.computeConditionalQuantile(x, y), 0.597413, 1e-6, 1e-6)
pt = ot.Point([0.1 * i + 0.05 for i in range(dim)])
ott.assert_almost_equal(
    copula.computeSequentialConditionalPDF(pt), [1.0, 1.06099249], 1e-6, 1e-8
)
resCDF = copula.computeSequentialConditionalCDF(pt)
ott.assert_almost_equal(resCDF, [0.05, 0.22026735], 1e-6, 1e-8)
ott.assert_almost_equal(
    copula.computeSequentialConditionalQuantile(resCDF), pt, 1e-8, 1e-8
)

# test BlockIndependentCopula.getMarginal in reverse
copula = ot.BlockIndependentCopula([ot.IndependentCopula(2), ot.NormalCopula(2)])
reverseMarginal = copula.getMarginal([3, 2, 1, 0])
assert reverseMarginal.getDimension() == 4
ott.assert_almost_equal(
    reverseMarginal.getCovariance(),
    ot.CovarianceMatrix(4, [1.0 / 12.0 if i == j else 0.0 for j in range(4) for i in range(4)]),
    1e-12,
    1e-12,
)

# check folding
d1 = ot.BlockIndependentCopula([ot.FrankCopula(3.0), ot.ClaytonCopula(2.0)])
d2 = ot.BlockIndependentCopula([ot.IndependentCopula(2), d1, ot.NormalCopula(2)])
assert len(d2.getCopulaCollection()) == 4

# setParameter round-trip and all-or-reject size check
paramCopula = ot.BlockIndependentCopula([ot.FrankCopula(3.0), ot.ClaytonCopula(2.0)])
reference = paramCopula.getParameter()
assert paramCopula.getParameterDescription() == [
    "copula_0_theta",
    "copula_1_theta",
]
paramCopula.setParameter([4.0, 5.0])
ott.assert_almost_equal(paramCopula.getParameter(), [4.0, 5.0])
with ott.assert_raises(Exception):
    paramCopula.setParameter([4.0])
assert list(paramCopula.getParameter()) == [4.0, 5.0]
with ott.assert_raises(Exception):
    paramCopula.setParameter([4.0, 5.0, 6.0])
assert list(paramCopula.getParameter()) == [4.0, 5.0]
paramCopula.setParameter(reference)
ott.assert_almost_equal(paramCopula.getParameter(), reference)

# --- BlockIndependentCopula: log-density path, probability offsets ---
bic = ot.BlockIndependentCopula([ot.IndependentCopula(1), ot.IndependentCopula(1)])
ott.assert_almost_equal(bic.computeLogPDF([0.5, 0.5]), 0.0, 1e-12, 0.0)
bic2 = ot.BlockIndependentCopula([ot.FrankCopula(3.0), ot.ClaytonCopula(2.0)])
prob = bic2.computeProbability(ot.Interval([0.0, 0.0, 0.5, 0.5], [1.0, 1.0, 1.0, 1.0]))
ref = ot.ClaytonCopula(2.0).computeProbability(ot.Interval([0.5, 0.5], [1.0, 1.0]))
ott.assert_almost_equal(prob, ref, 1e-12, 0.0)
assert prob < 1.0
