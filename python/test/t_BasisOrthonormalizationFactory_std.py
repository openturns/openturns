#! /usr/bin/env python

import os

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()


def makeBasis(functions, measure):
    """Expose a collection of functions and its measure as an OrthogonalBasis."""
    return ot.OrthogonalBasis(
        otexp.FiniteOrthogonalFunctionFactory(functions, measure)
    )


# Check the parameter constructor
dim = 2
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)

# Build initial basis: tensorized Hermite polynomials
refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
initialFunctions = [refBasis.build(i) for i in range(6)]
initialBasis = makeBasis(initialFunctions, distribution)
factory = otexp.BasisOrthonormalizationFactory(initialBasis, len(initialFunctions))

# Check that the factory returns the correct measure, basis and size
measured_dist = factory.getMeasure()
ott.assert_almost_equal(measured_dist.getMean(), distribution.getMean())
ott.assert_almost_equal(measured_dist.getCovariance(), distribution.getCovariance())

assert factory.getSize() == len(initialFunctions)
factoryAlgo = factory.getOrthonormalizationAlgorithm()
assert factoryAlgo.getOrthogonalBasis() == initialBasis
assert factoryAlgo.getSize() == len(initialFunctions)

# Build orthonormal functions, the first one is the constant 1
x = [0.5] * dim
kMax = len(initialFunctions)
functions = [factory.build(k) for k in range(kMax)]
ott.assert_almost_equal(functions[0](x), [1.0])

# An OrthogonalBasis without an explicit FiniteOrthogonalFunctionFactory is
# also valid: the Hermite basis is already orthonormal with respect to its
# normal measure.
normalBasis = ot.OrthogonalBasis(
    ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
)
normalFactory = otexp.BasisOrthonormalizationFactory(normalBasis, kMax)
assert normalFactory.getMeasure().getDimension() == dim
ott.assert_almost_equal(normalFactory.build(0)(x), [1.0])

# Check orthonormality with an independent Gauss-Legendre rule
integrationAlgo = ot.IntegrationAlgorithm(ot.GaussLegendre([12] * dim))
M = ot.SymmetricMatrix(kMax)
for m in range(kMax):
    for n in range(m + 1):

        def wrapper(x):
            return functions[m](x) * functions[n](x)[0] * distribution.computePDF(x)

        kernel = ot.PythonFunction(dim, 1, wrapper)
        value = integrationAlgo.integrate(kernel, distribution.getRange())[0]
        if abs(value) >= 1.0e-6:
            M[m, n] = value
ott.assert_almost_equal(M, ot.IdentityMatrix(kMax))

# Check coefficient matrix
C = factory.getCoefficients()
assert C.getDimension() == kMax

# Test with a measure-adapted experiment, wrapped as an integration algorithm
experiment = ot.GaussProductExperiment(distribution, [12] * dim)
factory2 = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
factory2.setIntegrationAlgorithm(ot.ExperimentIntegration(experiment))
f0 = factory2.build(0)
ott.assert_almost_equal(f0(x), [1.0])
assert "ExperimentIntegration" in repr(factory2.getIntegrationAlgorithm())
wrappedRule = ot.ExperimentIntegration(experiment)
assert wrappedRule.getWeightedExperiment().getSize() == experiment.getSize()

# An experiment whose instrumental distribution has a smaller support than the
# target is rejected: the missing part of the measure would be silently ignored.
badExperiment = ot.GaussProductExperiment(
    ot.JointDistribution([ot.Uniform(5.0, 6.0)] * dim), [4] * dim
)
with ott.assert_raises(TypeError):
    badFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
    badFactory.setIntegrationAlgorithm(ot.ExperimentIntegration(badExperiment))
    badFactory.build(0)

# A partially covering instrumental distribution is also rejected.
partialExperiment = ot.GaussProductExperiment(
    ot.JointDistribution([ot.Uniform(0.0, 0.5)] * dim), [4] * dim
)
with ott.assert_raises(TypeError):
    partialFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
    partialFactory.setIntegrationAlgorithm(ot.ExperimentIntegration(partialExperiment))
    partialFactory.build(0)

# A continuous experiment that integrates another measure is rejected even
# when its range contains the target range: the weights would produce a basis
# orthonormal for the experiment measure, not for the target measure.
wrongMeasureExperiment = ot.GaussProductExperiment(ot.Normal(dim), [4] * dim)
with ott.assert_raises(TypeError):
    wrongMeasureFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
    wrongMeasureFactory.setIntegrationAlgorithm(
        ot.ExperimentIntegration(wrongMeasureExperiment)
    )
    wrongMeasureFactory.build(0)

# The check also applies to importance sampling experiments: the importance
# distribution is the instrumental distribution.
smallImportance = ot.ImportanceSamplingExperiment(
    distribution,
    ot.JointDistribution([ot.Uniform(0.25, 0.75)] * dim),
    1000,
)
with ott.assert_raises(TypeError):
    importanceFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
    importanceFactory.setIntegrationAlgorithm(
        ot.ExperimentIntegration(smallImportance)
    )
    importanceFactory.build(0)

# An unbounded target cannot be covered by a bounded instrumental support.
unboundedDistribution = ot.JointDistribution([ot.Normal()] * dim)
unboundedBasis = makeBasis(initialFunctions, unboundedDistribution)
boundedExperiment = ot.GaussProductExperiment(
    ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * dim), [4] * dim
)
with ott.assert_raises(TypeError):
    unboundedFactory = otexp.BasisOrthonormalizationFactory(unboundedBasis, kMax)
    unboundedFactory.setIntegrationAlgorithm(
        ot.ExperimentIntegration(boundedExperiment)
    )
    unboundedFactory.build(0)

# A discrete experiment is exempt: its distribution is the empirical measure
# supported by the nodes, whose range is the convex hull of the nodes. A
# Gauss-Legendre rule mapped to the interior of the target support is valid
# even though its extreme nodes are interior to the support.
legendre = ot.GaussLegendre([12] * dim)
fixedNodes = ot.Sample(
    [[0.5 * (x[j] + 1.0) for j in range(dim)] for x in legendre.getNodes()]
)
fixedWeights = ot.Point(legendre.getWeights())
fixedExperiment = ot.FixedExperiment(fixedNodes, fixedWeights)
fixedFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
fixedFactory.setIntegrationAlgorithm(ot.ExperimentIntegration(fixedExperiment))
ott.assert_almost_equal(fixedFactory.build(0)(x), [1.0])

# Test with a custom integration algorithm for the Gram matrix
integrator = ot.IntegrationAlgorithm(ot.GaussLegendre([12] * dim))
factory4 = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
factory4.setIntegrationAlgorithm(integrator)
assert "GaussLegendre" in repr(factory4.getIntegrationAlgorithm())
functions4 = [factory4.build(k) for k in range(kMax)]
M4 = ot.SymmetricMatrix(kMax)
for m in range(kMax):
    for n in range(m + 1):

        def wrapper4(x):
            return functions4[m](x) * functions4[n](x)[0] * distribution.computePDF(x)

        kernel4 = ot.PythonFunction(dim, 1, wrapper4)
        value4 = integrationAlgo.integrate(kernel4, distribution.getRange())[0]
        if abs(value4) >= 1.0e-6:
            M4[m, n] = value4
ott.assert_almost_equal(M4, ot.IdentityMatrix(kMax))

# The rank and coefficient thresholds are relative: rescaling the initial
# functions must not change the orthonormal basis, on both factorization paths.
for scale in (1.0e-12, 1.0e12):
    scaledFunctions = [ot.LinearCombinationFunction([f], [scale]) for f in initialFunctions]
    scaledBasis = makeBasis(scaledFunctions, distribution)
    scaledFactory = otexp.BasisOrthonormalizationFactory(scaledBasis, kMax)
    scaledFunctions = [scaledFactory.build(k) for k in range(kMax)]
    ott.assert_almost_equal(scaledFunctions[0](x), [1.0])
    scaledAlgo = otexp.BasisOrthonormalizationAlgorithm(scaledBasis, kMax)
    scaledAlgo.setIntegrationAlgorithm(integrator)
    ott.assert_almost_equal(scaledAlgo.getOrthonormalFunctions()[0](x), [1.0])

# Public accessors of the algorithm
algoAccess = otexp.BasisOrthonormalizationAlgorithm(initialBasis, kMax)
algoAccess.run()
assert algoAccess.getMeasure().getDimension() == dim
assert algoAccess.getOrthogonalBasis() == initialBasis
assert algoAccess.getSize() == kMax
algoAccess.setIntegrationAlgorithm(integrator)
assert "GaussLegendre" in repr(algoAccess.getIntegrationAlgorithm())

# Public accessors of the factory
factoryCtor = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
factoryCtor.setIntegrationAlgorithm(integrator)
ott.assert_almost_equal(factoryCtor.build(0)(x), [1.0])
factoryAccess = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
factoryAccess.setIntegrationAlgorithm(integrator)
assert "GaussLegendre" in repr(factoryAccess.getIntegrationAlgorithm())
assert factoryAccess.getSize() == kMax
algoRef = factoryAccess.getOrthonormalizationAlgorithm()
assert algoRef.getOrthogonalBasis() == initialBasis
assert algoRef.getSize() == kMax

# The default constructor is only a placeholder for persistence.
defaultFactory = otexp.BasisOrthonormalizationFactory()
assert defaultFactory.getSize() == 0
with ott.assert_raises(TypeError):
    defaultFactory.build(0)
with ott.assert_raises(TypeError):
    defaultFactory.buildQuadratureRule(1)

# The size must be positive and must not exceed the number of available
# basis elements. A basis without a measure is rejected.
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationAlgorithm(initialBasis, 0)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationFactory(initialBasis, 0)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationAlgorithm(initialBasis, kMax + 1)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationFactory(initialBasis, kMax + 1)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationAlgorithm(ot.OrthogonalBasis(), 1)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationFactory(ot.OrthogonalBasis(), 1)

# Save/load round trip
studyFile = "basis_orthonormalization_factory.xml"
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager(studyFile))
study.add("factory", factory)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager(studyFile))
loadedStudy.load()
loadedFactory = otexp.BasisOrthonormalizationFactory()
loadedStudy.fillObject("factory", loadedFactory)
ott.assert_almost_equal(loadedFactory.build(0)(x), [1.0])
assert loadedFactory.getCoefficients().getDimension() == kMax
assert loadedFactory.getSize() == kMax
assert loadedFactory.getOrthonormalizationAlgorithm().getOrthogonalBasis() == initialBasis
assert loadedFactory.getOrthonormalizationAlgorithm().getSize() == kMax
os.remove(studyFile)

# The default tensor rule is capped so that a high-dimensional measure does
# not materialize a huge tensor product.
ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 100)
try:
    cappedFactory = otexp.BasisOrthonormalizationFactory(initialBasis, kMax)
    cappedFunctions = [cappedFactory.build(k) for k in range(kMax)]
    MCapped = ot.SymmetricMatrix(3)
    for m in range(3):
        for n in range(m + 1):

            def wrapperCapped(x):
                return cappedFunctions[m](x) * cappedFunctions[n](x)[0] * distribution.computePDF(x)

            kernelCapped = ot.PythonFunction(dim, 1, wrapperCapped)
            valueCapped = integrationAlgo.integrate(kernelCapped, distribution.getRange())[0]
            if abs(valueCapped) >= 1.0e-6:
                MCapped[m, n] = valueCapped
    ott.assert_almost_equal(MCapped, ot.IdentityMatrix(3))
finally:
    ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 1000000)

# A null tensor budget is rejected
ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 0)
try:
    with ott.assert_raises(TypeError):
        otexp.BasisOrthonormalizationFactory(initialBasis, kMax).build(0)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 1000000)

# A budget that cannot even hold the order-2 tensor rule is rejected instead
# of materializing 2^dimension nodes
ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 1)
try:
    with ott.assert_raises(TypeError):
        otexp.BasisOrthonormalizationFactory(initialBasis, kMax).build(0)
finally:
    ot.ResourceMap.SetAsUnsignedInteger("BasisOrthonormalizationAlgorithm-MaximumTensorNodes", 1000000)

# Test with a dependent measure: default integration uses Gauss-Legendre on the
# numerical range with the density folded into the weights
correlation = ot.CorrelationMatrix(dim)
correlation[0, 1] = 0.5
depDistribution = ot.JointDistribution(
    [ot.Beta(2.0, 3.0, 0.0, 1.0)] * dim, ot.NormalCopula(correlation)
)
depBasis = makeBasis(initialFunctions, depDistribution)
factoryDep = otexp.BasisOrthonormalizationFactory(depBasis, kMax)
functionsDep = [factoryDep.build(k) for k in range(kMax)]
MDep = ot.SymmetricMatrix(kMax)
# The Normal copula density is not polynomial: a 12-point Gauss-Legendre rule
# leaves a ~2e-4 error on this check, while 60 points bring it below 2e-7.
integrationAlgoDep = ot.IntegrationAlgorithm(ot.GaussLegendre([60] * dim))
for m in range(kMax):
    for n in range(m + 1):

        def wrapperDep(x):
            return functionsDep[m](x) * functionsDep[n](x)[0] * depDistribution.computePDF(x)

        kernelDep = ot.PythonFunction(dim, 1, wrapperDep)
        valueDep = integrationAlgoDep.integrate(kernelDep, depDistribution.getRange())[0]
        if abs(valueDep) >= 1.0e-6:
            MDep[m, n] = valueDep
ott.assert_almost_equal(MDep, ot.IdentityMatrix(kMax))

# A dependent elliptical measure whose standard distribution is not normal has
# no reliable Gauss-Legendre fallback: truncating its heavy tails loses the
# moments, so the default integration rule is rejected.
studentDistribution = ot.Student(3, [0.0] * dim, [1.0] * dim, correlation)
studentBasis = makeBasis(initialFunctions, studentDistribution)
with ott.assert_raises(TypeError):
    otexp.BasisOrthonormalizationFactory(studentBasis, kMax).build(0)

# Linearly dependent initial functions are rejected on both routes
duplicateFunctions = [initialFunctions[0], initialFunctions[0]]
duplicateBasis = makeBasis(duplicateFunctions, distribution)
with ott.assert_raises(TypeError):
    badAlgo = otexp.BasisOrthonormalizationAlgorithm(duplicateBasis, 2)
    badAlgo.getCoefficients()
with ott.assert_raises(TypeError):
    badAlgo2 = otexp.BasisOrthonormalizationAlgorithm(duplicateBasis, 2)
    badAlgo2.setIntegrationAlgorithm(integrator)
    badAlgo2.getCoefficients()
