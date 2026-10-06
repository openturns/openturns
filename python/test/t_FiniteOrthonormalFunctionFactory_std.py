#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp

ot.TESTPREAMBLE()

# Check the parameter constructor
dim = 2
distribution = ot.JointDistribution([ot.Uniform(0.0, 1.0)] * dim)

# Build initial basis: tensorized Hermite polynomials
refBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()] * dim)
initialBasis = [refBasis.build(i) for i in range(6)]

factory = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution)

# Check that the factory returns the correct measure and functions
measured_dist = factory.getMeasure()
ott.assert_almost_equal(measured_dist.getMean(), distribution.getMean())
ott.assert_almost_equal(measured_dist.getCovariance(), distribution.getCovariance())

initial_basis_check = factory.getFunctionsCollection()
assert len(initial_basis_check) == len(initialBasis)

# Build orthonormal functions, the first one is the constant 1
x = [0.5] * dim
kMax = len(initialBasis)
functions = [factory.build(k) for k in range(kMax)]
ott.assert_almost_equal(functions[0](x), [1.0])

# Check orthonormality via Gauss-Legendre integration
M = ot.SymmetricMatrix(kMax)
integrationAlgo = ot.GaussLegendre([48] * dim)
for m in range(kMax):
    for n in range(m + 1):

        def wrapper(x):
            return functions[m](x) * functions[n](x)[0] * distribution.computePDF(x)

        kernel = ot.PythonFunction(distribution.getDimension(), 1, wrapper)
        value = integrationAlgo.integrate(kernel, distribution.getRange())[0]
        if abs(value) >= 1.0e-6:
            M[m, n] = value
ott.assert_almost_equal(M, ot.IdentityMatrix(kMax))

# Check coefficient matrix (values below 1e-6 are numerical noise: no print)
C = factory.getCoefficients()
assert C.getDimension() == kMax

# Test with custom experiment
experiment = ot.GaussProductExperiment(distribution, [12] * dim)
factory2 = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution, experiment)
f0 = factory2.build(0)
ott.assert_almost_equal(f0(x), [1.0])

# Test with a reference-cube experiment
refExperiment = ot.GaussProductExperiment(ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * dim), [12] * dim)
factoryRef = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution, refExperiment)
functionsRef = [factoryRef.build(k) for k in range(kMax)]
MRef = ot.SymmetricMatrix(kMax)
for m in range(kMax):
    for n in range(m + 1):

        def wrapperRef(x):
            return functionsRef[m](x) * functionsRef[n](x)[0] * distribution.computePDF(x)

        kernelRef = ot.PythonFunction(distribution.getDimension(), 1, wrapperRef)
        valueRef = integrationAlgo.integrate(kernelRef, distribution.getRange())[0]
        if abs(valueRef) >= 1.0e-6:
            MRef[m, n] = valueRef
ott.assert_almost_equal(MRef, ot.IdentityMatrix(kMax))

# An experiment wrapped as an integration algorithm takes the experiment route
factoryW = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution)
factoryW.setIntegrationAlgorithm(ot.ExperimentIntegration(experiment))
ott.assert_almost_equal(factoryW.build(0)(x), [1.0])

# An experiment on another range is rejected
with ott.assert_raises(TypeError):
    badExperiment = ot.GaussProductExperiment(ot.JointDistribution([ot.Uniform(5.0, 6.0)] * dim), [4] * dim)
    badFactory = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution, badExperiment)
    badFactory.build(0)

# Test with a custom integration algorithm for the Gram matrix
integrator = ot.IntegrationAlgorithm(ot.GaussLegendre([12] * dim))
factory4 = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution)
factory4.setIntegrationAlgorithm(integrator)
assert factory4.getIntegrationAlgorithm().getImplementation().getClassName() == 'GaussLegendre'
functions4 = [factory4.build(k) for k in range(kMax)]
M4 = ot.SymmetricMatrix(kMax)
for m in range(kMax):
    for n in range(m + 1):

        def wrapper4(x):
            return functions4[m](x) * functions4[n](x)[0] * distribution.computePDF(x)

        kernel4 = ot.PythonFunction(distribution.getDimension(), 1, wrapper4)
        value4 = integrationAlgo.integrate(kernel4, distribution.getRange())[0]
        if abs(value4) >= 1.0e-6:
            M4[m, n] = value4
ott.assert_almost_equal(M4, ot.IdentityMatrix(kMax))

# Test default constructor + setMeasureAndFunctions
factory3 = otexp.FiniteOrthonormalFunctionFactory()
factory3.setMeasureAndFunctions(distribution, initialBasis)
factory3.setExperiment(experiment)
f3 = factory3.build(0)
ott.assert_almost_equal(f3(x), [1.0])

# The mapping sends the support back to the reference cube
algoRef = factory.getOrthonormalizationAlgorithm()
ott.assert_almost_equal(algoRef.getMapping()(x), [0.0] * dim)

# Test with a dependent measure: default integration uses Gauss-Legendre
correlation = ot.CorrelationMatrix(dim)
correlation[0, 1] = 0.5
depDistribution = ot.JointDistribution([ot.Beta(2.0, 3.0, 0.0, 1.0)] * dim, ot.NormalCopula(correlation))
factoryDep = otexp.FiniteOrthonormalFunctionFactory(initialBasis, depDistribution)
functionsDep = [factoryDep.build(k) for k in range(kMax)]
MDep = ot.SymmetricMatrix(kMax)
for m in range(kMax):
    for n in range(m + 1):

        def wrapperDep(x):
            return functionsDep[m](x) * functionsDep[n](x)[0] * depDistribution.computePDF(x)

        kernelDep = ot.PythonFunction(distribution.getDimension(), 1, wrapperDep)
        valueDep = integrationAlgo.integrate(kernelDep, depDistribution.getRange())[0]
        if abs(valueDep) >= 1.0e-6:
            MDep[m, n] = valueDep
ott.assert_almost_equal(MDep, ot.IdentityMatrix(kMax))

# Linearly dependent initial functions are rejected on both routes
with ott.assert_raises(TypeError):
    badAlgo = otexp.FiniteOrthonormalizationAlgorithm([initialBasis[0], initialBasis[0]], distribution)
    badAlgo.getCoefficients()
with ott.assert_raises(TypeError):
    badAlgo2 = otexp.FiniteOrthonormalizationAlgorithm([initialBasis[0], initialBasis[0]], distribution, integrator)
    badAlgo2.getCoefficients()
