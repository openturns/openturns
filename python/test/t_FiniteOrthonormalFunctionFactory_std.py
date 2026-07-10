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
print("Factory=", factory)
print("Factory=", repr(factory))

# Check that the factory returns the correct measure and functions
measured_dist = factory.getMeasure()
ott.assert_almost_equal(measured_dist.getMean(), distribution.getMean())
ott.assert_almost_equal(measured_dist.getCovariance(), distribution.getCovariance())

initial_basis_check = factory.getFunctionsCollection()
print("Number of initial functions:", len(initial_basis_check))

# Build orthonormal functions
x = [0.5] * dim
kMax = len(initialBasis)
functions = list()
for k in range(kMax):
    fk = factory.build(k)
    functions.append(fk)
    value = fk(x)
    print(f"OrthonormalFunction_{k}({x})={value}")

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

# Check coefficient matrix
C = factory.getCoefficients()
print("Coefficients=\n", C)
assert C.getDimension() == kMax

# Test buildQuadrature
nodes, weights = factory.buildQuadrature(3)
print(f"Quadrature: {len(nodes)} nodes, weights sum={sum(weights):.6f}")
assert len(nodes) > 0
assert len(weights) == len(nodes)

# Verify exact integration of first n orthonormal functions
for k in range(3):
    integral = sum(weights[i] * functions[k](nodes[i])[0] for i in range(len(nodes)))
    if k == 0:
        ott.assert_almost_equal(integral, 1.0, 1e-3, 1e-3)
    else:
        ott.assert_almost_equal(integral, 0.0, 1e-3, 1e-3)

# Test with custom experiment
experiment = ot.GaussProductExperiment(distribution, [12] * dim)
factory2 = otexp.FiniteOrthonormalFunctionFactory(initialBasis, distribution, experiment)
f0 = factory2.build(0)
print("With custom experiment:", f0(x))

# Test default constructor + setMeasureAndFunctions
factory3 = otexp.FiniteOrthonormalFunctionFactory()
factory3.setMeasureAndFunctions(distribution, initialBasis)
factory3.setExperiment(experiment)
f3 = factory3.build(0)
print("setMeasureAndFunctions OK")
