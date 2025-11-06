#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import openturns.experimental as otexp
from math import log10

ot.TESTPREAMBLE()

# Define the model
dimension = 3
input_names = ["x1", "x2", "x3"]
formulas = ["cos(x1 + x2 + x3)"]
model = ot.SymbolicFunction(input_names, formulas)

# Sample noisy data
distribution = ot.Normal(dimension)
samplesize = 100
x = distribution.getSample(samplesize)
y = model(x) + ot.Normal(0.0, 0.1).getSample(samplesize)

# Build the surrogate model
basis = ot.LinearBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential(dimension)
covarianceModel.activateNuggetFactor(True)

# Gaussian process fitter
fitter_algo = otexp.GaussianProcessFitter(x, y, covarianceModel, basis)
fitter_algo.run()
fitter_result = fitter_algo.getResult()

algo = otexp.GaussianProcessRegression(fitter_result)
algo.run()
result = algo.getResult()
optimized_covariance_model = result.getCovarianceModel()
ott.assert_almost_equal(
    optimized_covariance_model.getScale(), [0.874623, 0.491866, 0.84785], 0.2, 0.0
)
ott.assert_almost_equal(
    log10(optimized_covariance_model.getNuggetFactor()), -0.782689, 0.25, 0.0
)

# Product covariance kernel
# (with SquaredExponential => identical to the first)
product = ot.ProductCovarianceModel([ot.SquaredExponential()] * dimension)
product.activateNuggetFactor(True)

fitter_algo_product = otexp.GaussianProcessFitter(x, y, product, basis)
fitter_algo_product.run()
fitter_result_product = fitter_algo_product.getResult()

algo_product = otexp.GaussianProcessRegression(fitter_result_product)
algo_product.run()
result_product = algo_product.getResult()
optimized_product_model = result_product.getCovarianceModel()
ott.assert_almost_equal(
    log10(optimized_product_model.getNuggetFactor()), -0.782689, 0.25, 0.0
)

# Isotropic covariance kernel
isotropic = ot.IsotropicCovarianceModel(ot.SquaredExponential(), dimension)
isotropic.activateNuggetFactor(True)

fitter_algo_iso = otexp.GaussianProcessFitter(x, y, isotropic, basis)
fitter_algo_iso.run()
fitter_result_iso = fitter_algo_iso.getResult()

algo_iso = otexp.GaussianProcessRegression(fitter_result_iso)
algo_iso.run()
result_iso = algo_iso.getResult()
optimized_isotropic_model = result_iso.getCovarianceModel()
ott.assert_almost_equal(
    log10(optimized_isotropic_model.getNuggetFactor()), -1.9, 0.25, 0.0
)
