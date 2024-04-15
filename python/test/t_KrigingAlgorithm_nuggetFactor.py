#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
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
basis = ot.ConstantBasisFactory(dimension).build()
covarianceModel = ot.SquaredExponential(dimension)
covarianceModel.activateNuggetFactor(True)
algo = ot.KrigingAlgorithm(x, y, covarianceModel, basis)
algo.run()
result = algo.getResult()
optimized_covariance_model = result.getCovarianceModel()
ott.assert_almost_equal(
    optimized_covariance_model.getScale(), [1.34] * dimension, 0.2, 0.0
)
ott.assert_almost_equal(
    log10(optimized_covariance_model.getNuggetFactor()), -1.9, 0.25, 0.0
)

# Product covariance kernel
# (with SquaredExponential => identical to the first)
product = ot.ProductCovarianceModel([ot.SquaredExponential()] * dimension)
product.activateNuggetFactor(True)
algo_product = ot.KrigingAlgorithm(x, y, product, basis)
algo_product.run()
result_product = algo_product.getResult()
optimized_product_model = result_product.getCovarianceModel()
ott.assert_almost_equal(
    optimized_product_model.getScale(), [1.34] * dimension, 0.2, 0.0
)
ott.assert_almost_equal(
    log10(optimized_product_model.getNuggetFactor()), -1.9, 0.25, 0.0
)

# Isotropic covariance kernel
isotropic = ot.IsotropicCovarianceModel(ot.SquaredExponential(), dimension)
isotropic.activateNuggetFactor(True)
algo_iso = ot.KrigingAlgorithm(x, y, isotropic, basis)
algo_iso.run()
result_iso = algo_iso.getResult()
optimized_isotropic_model = result_iso.getCovarianceModel()
ott.assert_almost_equal(optimized_isotropic_model.getScale(), [1.34], 0.2, 0.0)
ott.assert_almost_equal(
    log10(optimized_isotropic_model.getNuggetFactor()), -1.9, 0.25, 0.0
)
