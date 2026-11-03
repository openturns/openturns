#!/usr/bin/env python

import os
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
copula = ot.NormalCopula(R)
distribution = ot.PointConditionalDistribution(copula, [1], [0.8])

print("Distribution ", repr(distribution))
print("Distribution ", distribution)

# Get mean and covariance
print("Mean= ", repr(distribution.getMean()))
print("Covariance= ", repr(distribution.getCovariance()))

# Is this distribution elliptical ?
print("Elliptical = ", distribution.isElliptical())

# Test for realization of distribution
oneRealization = distribution.getRealization()
print("oneRealization=", repr(oneRealization))

print("dim=", distribution.getDimension())
print("range=", distribution.getRange())
print("mean=", distribution.getMean())
print("cov=", distribution.getCovariance())
print("marginal=", distribution.getMarginal([1]))

ot.Log.Show(ot.Log.TRACE)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipGradient()  # slow
validation.skipDependenceMeasures()  # slow Spearman/Kendall on 1M-sample
validation.setCDFSamplingSize(1)
validation.run()

# no conditioning
distribution = ot.PointConditionalDistribution(copula, [], [])
simplified = distribution.getSimplifiedVersion()
print(distribution.getSimplifiedVersion())
ott.assert_almost_equal(simplified, copula)

# discrete case
discrete = ot.Multinomial(5, [0.25] * 3)
# the default NormalCopula range adaptation integrates the grade correlation
# of the distribution (slow here): the marginal range already covers the
# conditional support, which is what is checked below
ot.ResourceMap.SetAsString("PointConditionalDistribution-RangeAdaptationMethod", "None")
distribution = ot.PointConditionalDistribution(discrete, [1], [1.0])
ot.ResourceMap.SetAsString(
    "PointConditionalDistribution-RangeAdaptationMethod", "NormalCopula"
)
print("dim=", distribution.getDimension())
assert distribution.getDimension() == 2, "wrong dimension"
print("range=", distribution.getRange())
print("mean=", distribution.getMean())
print("cov=", distribution.getCovariance())
print("marginal=", distribution.getMarginal([1]))
print("support=", distribution.getSupport())
assert distribution.isDiscrete(), "wrong isDiscrete"
ott.assert_almost_equal(distribution.getMean(), [4 / 3] * 2)
validation = ott.DistributionValidation(distribution)
validation.skipMoments()  # slow
validation.skipCorrelation()  # slow
validation.skipGradient()  # slow
validation.skipParameters()  # integer parameter
validation.skipDependenceMeasures()  # slow
validation.setCDFSamplingSize(1)
validation.run()

# special case for Normal
normal = ot.Normal([0.0] * 3, R)
distribution = ot.PointConditionalDistribution(normal, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
simplified_ref = ot.Normal(
    [1.8] * 2, [0.43589] * 2, ot.CorrelationMatrix([[1.0, 0.473684], [0.473684, 1.0]])
)
ott.assert_almost_equal(simplified, simplified_ref)

# ensure services are ok
mean_ref = simplified_ref.getMean()
ott.assert_almost_equal(distribution.getMean(), mean_ref)
ott.assert_almost_equal(
    distribution.getStandardDeviation(), simplified_ref.getStandardDeviation()
)
ott.assert_almost_equal(
    distribution.computePDF(mean_ref), simplified_ref.computePDF(mean_ref)
)
ott.assert_almost_equal(
    distribution.computeCDF(mean_ref), simplified_ref.computeCDF(mean_ref)
)

# special case for Student
student = ot.Student(2.5, [0.4] * 3, [1.2] * 3, R)
distribution = ot.PointConditionalDistribution(student, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
ott.assert_almost_equal(simplified.getMarginal(0), ot.Student(3.5, 1.84, 0.578273))
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.540416310512559)

# special case for Mixture
mixture = ot.Mixture([ot.Normal(2), ot.Normal(2)], [0.3, 0.7])
distribution = ot.PointConditionalDistribution(mixture, [1], [2.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.398942)

# special case for KernelMixture
kernel = ot.Uniform()
sample = ot.Normal(2).getSample(5)
bandwidth = [1.0] * 2
kernelMixture = ot.KernelMixture(kernel, bandwidth, sample)
distribution = ot.PointConditionalDistribution(kernelMixture, [1], [1.0])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF([0]), 0.25)

# ratio of uniform method initialization is harder for Dirichlet
R = ot.CorrelationMatrix(3, [1.0, 0.5, 0.1, 0.5, 1.0, 0.2, 0.1, 0.2, 1.0])
core = ot.Dirichlet([1, 2, 3, 4])
joint = ot.JointDistribution([ot.Exponential()] * 3, core)
# ratio of uniform sampling needs the default NormalCopula range adaptation
# here (other range strategies give a range unsuitable for sampler init)
distribution = ot.PointConditionalDistribution(joint, [1], [2.0])
sample = distribution.getSample(10)

# special case for EmpiricalBernsteinCopula
sample = normal.getSample(10)
bernstein = ot.EmpiricalBernsteinCopula(sample, 5)
distribution = ot.PointConditionalDistribution(bernstein, [1], [0.5])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"

# Boundary values of an empirical copula sample are clipped to the bin
# range before the beta densities are evaluated
boundarySample = ot.Sample([[0.0, 0.0], [1.0, 0.5], [0.5, 1.0], [0.25, 0.75]])
boundaryCopula = ot.EmpiricalBernsteinCopula(boundarySample, 2, True)
distribution = ot.PointConditionalDistribution(boundaryCopula, [0], [0.5])
simplified = distribution.getSimplifiedVersion()
assert simplified.getName() == "Mixture", "wrong type"
assert distribution.computePDF([0.25]) > 0.0

# special case for BlockIndependentDistribution
blockIndep = ot.BlockIndependentDistribution([normal, student, mixture])
# test a conditioning which remove the first block, modify the second block and let the last block unchanged
distribution = ot.PointConditionalDistribution(
    blockIndep, [1, 3, 0, 2], [0.5, 0.5, 0.5, 0.5]
)
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "BlockIndependentDistribution", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.14676486183286414)
# test a conditioning which remove all but the last block
# As the last block can be simplified, it is its actual simplification which is used
distribution = ot.PointConditionalDistribution(
    blockIndep, [0, 1, 2, 3, 4, 5, 6], [0.5] * 7
)
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "Mixture", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 0.398942)

# special case for BlockIndependentCopula
R = ot.CorrelationMatrix(3)
R[1, 0] = R[2, 0] = R[2, 1] = 0.9
blockIndep = ot.BlockIndependentCopula([ot.GumbelCopula(), ot.NormalCopula(R)])
# test a conditioning which remove the first block, modify the second block and let the last block unchanged
distribution = ot.PointConditionalDistribution(blockIndep, [1, 3], [0.4, 0.5])
simplified = distribution.getSimplifiedVersion()
print(simplified)
assert simplified.getName() == "BlockIndependentDistribution", "wrong type"
ott.assert_almost_equal(distribution.computePDF(distribution.getMean()), 9.01202)

# negative bound sampling case
ot.ResourceMap.SetAsBool("PointConditionalDistribution-UseSimplifiedVersion", False)
distribution = ot.PointConditionalDistribution(copula - 1.0, [1], [-0.8])
# 2000 draws are enough: tolerance is loose (1e-2) vs Monte Carlo error
ott.assert_almost_equal(
    distribution.getSample(2000).computeMean(), [-0.75] * 2, 1e-2, 1e-2
)

# Verify swapped bivariate copula uses generic methods
# With IndependentCopula, the conditional distribution is Uniform(0,1)
indep = ot.IndependentCopula(2)
cond = ot.PointConditionalDistribution(indep, [1], [0.3])
ott.assert_almost_equal(cond.computeCDF([0.5]), 0.5)
ott.assert_almost_equal(cond.computePDF([0.5]), 1.0)

# Test computeSequentialConditional* with custom ordering
copula = ot.NormalCopula(ot.CorrelationMatrix([[1.0, 0.8], [0.8, 1.0]]))
pcd = ot.PointConditionalDistribution(copula, [1], [0.3])
q = ot.Point([0.6])
# 2-arg with full identity ordering should match 1-arg version
ott.assert_almost_equal(pcd.computeSequentialConditionalCDF(q, [0]), pcd.computeSequentialConditionalCDF(q), 1e-5, 1e-5)
ott.assert_almost_equal(pcd.computeSequentialConditionalPDF(q, [0]), pcd.computeSequentialConditionalPDF(q), 1e-5, 1e-5)
ott.assert_almost_equal(pcd.computeSequentialConditionalQuantile(q, [0]), pcd.computeSequentialConditionalQuantile(q), 1e-5, 1e-5)

# Generic path with a discrete conditional: the lower-bound atom is included
joint = ot.JointDistribution([ot.Poisson(1.0), ot.Poisson(1.0)], ot.FrankCopula(3.0))
cond = ot.PointConditionalDistribution(joint, [1], [0.0])
# P(X0 in [0, 1] | X1 = 0) sums the conditional PMF over both atoms
expected = (joint.computePDF([0.0, 0.0]) + joint.computePDF([1.0, 0.0])) / joint.getMarginal(1).computePDF(0.0)
ott.assert_almost_equal(cond.computeProbability(ot.Interval([-1.0], [1.0])), expected, 1e-12, 0.0)

# Failed setParameter leaves the distribution unchanged
joint = ot.JointDistribution([ot.Normal(), ot.Uniform()])
cond = ot.PointConditionalDistribution(joint, [1], [0.0])
before = cond.getParameter()
pdfBefore = cond.computePDF([0.0])
bad = ot.Point(before)
bad[bad.getSize() - 1] = 5.0
with ott.assert_raises(TypeError):
    cond.setParameter(bad)
ott.assert_almost_equal(cond.getParameter(), before, 0.0, 0.0)
ott.assert_almost_equal(cond.computePDF([0.0]), pdfBefore, 0.0, 0.0)

# --- PointConditionalDistribution: Student conditional scale is linear ---
# the simplified Student formula is under test: the host file above disables it
ot.ResourceMap.SetAsBool("PointConditionalDistribution-UseSimplifiedVersion", True)
R = ot.CorrelationMatrix(2)
R[0, 1] = 0.5
pcdStudent = ot.PointConditionalDistribution(
    ot.Student(5.0, [0.0, 0.0], [1.0, 1.0], R), [1], [0.5]
)
ott.assert_almost_equal(pcdStudent.getMean(), [0.25], 1e-12, 0.0)
ott.assert_almost_equal(
    pcdStudent.getCovariance(), ot.CovarianceMatrix(1, [0.984375]), 1e-12, 0.0
)
# covariance constructor honors its documented contract
covStudent = ot.Student(6.0, [0.25], ot.CovarianceMatrix(1, [0.984375]))
ott.assert_almost_equal(
    covStudent.getCovariance(), ot.CovarianceMatrix(1, [0.984375]), 1e-12, 0.0
)
with ott.assert_raises(Exception):
    ot.Student(1.5, [0.0], ot.CovarianceMatrix(1, [1.0]))
# ... including for singular matrices as before
singStudent = ot.Student(
    4.2, [0.0] * 3, ot.CovarianceMatrix([[1.0, 1.0, 0.0], [1.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
)
ott.assert_almost_equal(singStudent.getMean(), [0.0] * 3, 0.0, 0.0)

# --- PointConditionalDistribution: parameter validation and persistence ---
studyFile = "myStudyBugFixPointConditional.xml"
pcd = ot.PointConditionalDistribution(ot.Normal(2), [1], [0.5])
pcdParam = pcd.getParameter()
pcdParam[-1] = 1.0e6
with ott.assert_raises(Exception):
    pcd.setParameter(pcdParam)
pcd.setIntegrationAlgorithm(ot.SimplicialCubature())
study = ot.Study(studyFile)
study.add("pcd", pcd)
study.save()
study2 = ot.Study(studyFile)
study2.load()
pcdR = ot.PointConditionalDistribution(ot.Normal(2), [1], [0.5])
study2.fillObject("pcd", pcdR)
os.remove(studyFile)
assert (
    pcdR.getIntegrationAlgorithm().getImplementation().getClassName()
    == "SimplicialCubature"
)
ott.assert_almost_equal(
    pcdR.computePDF([0.0]), pcd.computePDF([0.0]), 1e-12, 0.0
)

# --- Coverage: validation, predicates, accessors, simplified paths ---
with ott.assert_raises(Exception):
    ot.PointConditionalDistribution(ot.Normal(2), [0], [0.0, 0.0])
with ott.assert_raises(Exception):
    ot.PointConditionalDistribution(ot.Normal(2), [5], [0.0])
jmix_cov = ot.JointDistribution([ot.Poisson(2.0), ot.Normal(), ot.Uniform()])
with ott.assert_raises(Exception):
    ot.PointConditionalDistribution(jmix_cov, [1], [0.0])
u2_cov = ot.JointDistribution([ot.Uniform(), ot.Uniform()])
with ott.assert_raises(Exception):
    ot.PointConditionalDistribution(u2_cov, [0], [5.0])
n_cov = ot.Normal([0.0] * 3, ot.CorrelationMatrix(3))
pcd_cov = ot.PointConditionalDistribution(n_cov, [1], [0.5])
assert pcd_cov.isContinuous()
assert not pcd_cov.isDiscrete()
assert pcd_cov.isElliptical()
assert pcd_cov.hasEllipticalCopula()
assert pcd_cov.getStandardDeviation().getSize() == 2
assert pcd_cov.getSkewness().getSize() == 2
assert pcd_cov.getKurtosis().getSize() == 2
assert pcd_cov.getMarginal(0).getDimension() == 1
assert pcd_cov.getMarginal([0]).getDimension() == 1
assert pcd_cov.getConditioningValues() == [0.5]
assert list(pcd_cov.getConditioningIndices()) == [1]
assert pcd_cov.getDistribution().getClassName() != ""
ot.ResourceMap.SetAsString("PointConditionalDistribution-RangeAdaptationMethod", "Normal")
assert pcd_cov.getRange().getDimension() == 2
ot.ResourceMap.SetAsString("PointConditionalDistribution-RangeAdaptationMethod", "NormalCopula")
assert pcd_cov == pcd_cov
assert pcd_cov == ot.PointConditionalDistribution(n_cov, [1], [0.5])
assert pcd_cov.computeQuantile(0.5).getSize() == 2
pcd1d_cov = ot.PointConditionalDistribution(ot.Normal(2), [1], [0.5])
assert isinstance(pcd1d_cov.computeScalarQuantile(0.5), float)
assert pcd_cov.computePDFGradient([0.0, 0.0]).getSize() >= 1
assert pcd_cov.computeCDFGradient([0.0, 0.0]).getSize() >= 1
assert pcd_cov.getIsoProbabilisticTransformation()(pcd_cov.getMean()).getSize() == 2
assert pcd_cov.getInverseIsoProbabilisticTransformation()([0.0, 0.0]).getSize() == 2
assert pcd_cov.computeDDF(pcd_cov.getMean()).getSize() == 2
assert isinstance(pcd_cov.computeLogPDF(pcd_cov.getMean()), float)
ot.ResourceMap.SetAsBool("PointConditionalDistribution-UseSimplifiedVersion", False)
with ott.assert_raises(Exception):
    ot.PointConditionalDistribution(ot.NormalCopula(ot.CorrelationMatrix(2)), [], [])
ot.ResourceMap.SetAsBool("PointConditionalDistribution-UseSimplifiedVersion", True)
n2_cov = ot.Normal(2)
jbern_cov = ot.JointDistribution(
    [ot.Normal(), ot.Normal()], ot.EmpiricalBernsteinCopula(n2_cov.getSample(10), 3)
)
pbern_cov = ot.PointConditionalDistribution(jbern_cov, [1], [0.0])
assert pbern_cov.getDimension() == 1
assert pbern_cov.computeProbability(ot.Interval([-10.0], [10.0])) >= 0.0
