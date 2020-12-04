#! /usr/bin/env python

from __future__ import print_function
import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def fitKriging(covarianceModel):
    '''
    Fit the parameters of a kriging metamodel. 
    '''
    coordinates = ot.Sample([[1.0,1.0],[5.0,1.0],[9.0,1.0], \
                         [1.0,3.5],[5.0,3.5],[9.0,3.5], \
                         [1.0,6.0],[5.0,6.0],[9.0,6.0]])
    observations = ot.Sample([[25.0],[25.0],[10.0],[20.0],[25.0],[20.0],[15.0],[25.0],[25.0]])
    basis = ot.ConstantBasisFactory(2).build()
    algo = ot.KrigingAlgorithm(coordinates, observations, covarianceModel, basis)
    algo.run()
    krigingResult = algo.getResult()
    return krigingResult


# Isotropic covariance model
myIsotropicKernel = ot.IsotropicCovarianceModel(ot.SquaredExponential(), 2)
krigingFittedCovarianceModel = fitKriging(myIsotropicKernel).getCovarianceModel()
ott.assert_almost_equal(krigingFittedCovarianceModel.getScale()[0], 2.86427, 0.0, 1e-4)
ott.assert_almost_equal(krigingFittedCovarianceModel.getAmplitude()[0], 6.65231, 0.0, 1e-4)
