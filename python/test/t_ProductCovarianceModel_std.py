#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


def test_active_parameter():
    # Define product of matern 1d
    cov_model_1d = ot.MaternModel([0.5], 2.5)
    print("1D Full parameter : ", cov_model_1d.getFullParameter())
    print("1D active cov. param.: ", [cov_model_1d.getFullParameterDescription()[
          i] for i in cov_model_1d.getActiveParameter()])
    print("Activate nu parameter")
    cov_model_1d.setActiveParameter([0, 1, 2])
    print("active cov. param.: ", [cov_model_1d.getFullParameterDescription()[
          i] for i in cov_model_1d.getActiveParameter()])

    print("Matern d-dimensional covariance as product")
    d = 3
    cov_model = ot.ProductCovarianceModel([cov_model_1d]*d)
    print("Full parameter : ", cov_model.getFullParameter())
    print("active cov. param.: ", [cov_model.getFullParameterDescription()[
          i] for i in cov_model.getActiveParameter()])
    print("Disable nu for marginals 0 & 1 parameter : ",
          cov_model.getFullParameter())
    cov_model.setActiveParameter([0, 1, 2, 3, 6])
    print("active cov. param.: ", [cov_model.getFullParameterDescription()[
          i] for i in cov_model.getActiveParameter()])
    print("Check that active parameter is correctly propagated")

    for k in range(3):
        print("Model ", k, " : active cov. param.: ", [cov_model.getCollection()[
              k].getFullParameterDescription()[i] for i in cov_model.getCollection()[k].getActiveParameter()])


def test_active_amplitude_parameter():
    # Define product of matern 1d
    model1 = ot.MaternModel([1.0], 2.5)
    print("Model 1 : ", model1.getFullParameterDescription())
    print("Activate nu parameter and disable sigma2")
    model1.setActiveParameter([0, 2])
    print("model1 active parameter: ", [model1.getFullParameterDescription()[
          i] for i in model1.getActiveParameter()])

    model2 = ot.ExponentiallyDampedCosineModel()
    print("Model 2 : ", model2.getFullParameterDescription())
    print("Activate freq parameter")
    model2.setActiveParameter([0, 1, 2])
    print("model2 active parameter: ", [model2.getFullParameterDescription()[
          i] for i in model2.getActiveParameter()])

    print("Product covariance model")
    d = 3
    cov_model = ot.ProductCovarianceModel([model1, model2])
    print("Full parameter : ", cov_model.getFullParameter())
    print("active cov. param.: ", [cov_model.getFullParameterDescription()[
          i] for i in cov_model.getActiveParameter()])


test_active_parameter()
test_active_amplitude_parameter()
