#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
from openturns.testing import assert_almost_equal

ot.TESTPREAMBLE()

sampleSize = 6
dimension = 1

f = ot.SymbolicFunction(['x0'], ['x0 * sin(x0)'])

X = ot.Sample([1.0, 3.0, 5.0, 6.0, 7.0, 8.0], dimension)
Y = f(X)

# Test 1
for prior in [ot.GeneralLinearModelAlgorithm.NONE, ot.GeneralLinearModelAlgorithm.JOINTLYROBUST, ot.GeneralLinearModelAlgorithm.REFERENCE]:
    for parametrization in [ot.CovarianceModelImplementation.STANDARD, ot.CovarianceModelImplementation.INVERSE, ot.CovarianceModelImplementation.LOGINVERSE]:

        ot.RandomGenerator.SetSeed(0)

        # create algorithm
        basis = ot.ConstantBasisFactory(dimension).build()
        covarianceModel = ot.SquaredExponential([1e-05], [4.11749])
        covarianceModel.setScaleParametrization(parametrization)

        algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis)
        algo.setScalePrior(prior)
        algo.run()

        # perform an evaluation
        result = algo.getResult()
        print('parameters=', result.getCovarianceModel().getParameter())
        #print("X=", X)
        #print("f(X)=", Y)

        assert_almost_equal(result.getMetaModel()(X), Y)
        assert_almost_equal(result.getResiduals(), [1.32804e-07], 1e-3, 1e-3)
        assert_almost_equal(result.getRelativeErrors(), [5.20873e-21])

        # Kriging variance is 0 on learning points
        var = result.getConditionalCovariance(X)

        # assert_almost_equal could not be applied to matrices
        # application to Point
        covariancePoint = ot.Point(var.getImplementation())
        theoricalVariance = ot.Point(sampleSize * sampleSize)
        assert_almost_equal(covariancePoint, theoricalVariance, 8.95e-7, 8.95e-7)

# Tests with MaternModel
scale = [1.0]
amplitude = [4.123456]
nu = 0.5

# Test A : prior = REFERENCE, parametrization = STANDARD
print("Test A : prior = REFERENCE, parametrization = STANDARD")
prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.STANDARD
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], 0.8296342228649921, 1.e-1) # Not accurate
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -11.164598619012276, 1.e-3)

# Test B : prior = REFERENCE, parametrization = INVERSE
print("Test B : prior = REFERENCE, parametrization = INVERSE")
prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.INVERSE
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], 0.006700808318183162, 1.e1) # Not accurate
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -9.508456344459574, 1.e-2)

# Test C : prior = REFERENCE, parametrization = LOGINVERSE
print("Test C : prior = REFERENCE, parametrization = LOGINVERSE")
prior = ot.GeneralLinearModelAlgorithm.REFERENCE
parametrization = ot.CovarianceModelImplementation.LOGINVERSE
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], -0.6160855351429757, 1.e-2)
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -10.959574974572401, 1.e-6)

# Test D : prior = JOINTLYROBUST, parametrization = STANDARD
print("Test D : prior = JOINTLYROBUST, parametrization = STANDARD")
prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.STANDARD
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], 0.10613379308918715, 1.e-4)
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -9.680803951403554, 1.e-8)

# Test E : prior = JOINTLYROBUST, parametrization = INVERSE
print("Test E : prior = JOINTLYROBUST, parametrization = INVERSE")
prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.INVERSE
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], 0.32282538656275017, 1.e-4)
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -11.688006015506188, 1.e-8)

# Test F : prior = JOINTLYROBUST, parametrization = LOGINVERSE
print("Test F : prior = JOINTLYROBUST, parametrization = LOGINVERSE")
prior = ot.GeneralLinearModelAlgorithm.JOINTLYROBUST
parametrization = ot.CovarianceModelImplementation.LOGINVERSE
covarianceModel = ot.MaternModel(scale, amplitude, nu)
covarianceModel.setScaleParametrization(parametrization)
algo = ot.KrigingAlgorithm(X, Y, covarianceModel, basis, False)
algo.setScalePrior(prior)
algo.run()
result = algo.getResult()
scaleOTGU = result.getCovarianceModel().getParameter()
assert_almost_equal(scaleOTGU[0], 1.5708551041440961, 1.e-6)
rllfunction = algo.getReducedLogLikelihoodFunction()
objective_value = rllfunction(scaleOTGU)
assert_almost_equal(objective_value[0], -11.638534909763555, 1.e-8)

