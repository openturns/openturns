#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


def test_active_parameter():
    # Define product of matern 1d
    cov_model_1d = ot.MaternModel([0.5], 2.5)
    print("1D Full parameter : ", cov_model_1d.getFullParameter())
    print(
        "1D active cov. param.: ",
        [
            cov_model_1d.getFullParameterDescription()[i]
            for i in cov_model_1d.getActiveParameter()
        ],
    )
    print("Activate nu parameter")
    cov_model_1d.setActiveParameter([0, 1, 2])
    print(
        "active cov. param.: ",
        [
            cov_model_1d.getFullParameterDescription()[i]
            for i in cov_model_1d.getActiveParameter()
        ],
    )

    print("Matern d-dimensional covariance as product")
    d = 3
    cov_model = ot.ProductCovarianceModel([cov_model_1d] * d)
    marginal0 = cov_model.getMarginal(0)
    assert marginal0.getInputDimension() == d, "wrong marginal input dim"
    assert marginal0.getOutputDimension() == 1, "wrong marginal output dim"
    print("Full parameter : ", cov_model.getFullParameter())
    print(
        "active cov. param.: ",
        [
            cov_model.getFullParameterDescription()[i]
            for i in cov_model.getActiveParameter()
        ],
    )
    print("Disable nu for marginals 0 & 1 parameter : ", cov_model.getFullParameter())
    cov_model.setActiveParameter([0, 1, 2, 3, 6])
    print(
        "active cov. param.: ",
        [
            cov_model.getFullParameterDescription()[i]
            for i in cov_model.getActiveParameter()
        ],
    )
    print("Check that active parameter is correctly propagated")

    for k in range(3):
        print(
            "Model ",
            k,
            " : active cov. param.: ",
            [
                cov_model.getCollection()[k].getFullParameterDescription()[i]
                for i in cov_model.getCollection()[k].getActiveParameter()
            ],
        )


def test_active_amplitude_parameter():
    # Define product of matern 1d
    model1 = ot.MaternModel([1.0], 2.5)
    print("Model 1 : ", model1.getFullParameterDescription())
    print("Activate nu parameter and disable sigma2")
    model1.setActiveParameter([0, 2])
    print(
        "model1 active parameter: ",
        [model1.getFullParameterDescription()[i] for i in model1.getActiveParameter()],
    )

    model2 = ot.ExponentiallyDampedCosineModel()
    print("Model 2 : ", model2.getFullParameterDescription())
    print("Activate freq parameter")
    model2.setActiveParameter([0, 1, 2])
    print(
        "model2 active parameter: ",
        [model2.getFullParameterDescription()[i] for i in model2.getActiveParameter()],
    )

    print("Product covariance model")
    cov_model = ot.ProductCovarianceModel([model1, model2])
    print("Full parameter : ", cov_model.getFullParameter())
    print(
        "active cov. param.: ",
        [
            cov_model.getFullParameterDescription()[i]
            for i in cov_model.getActiveParameter()
        ],
    )


def test_parameters_iso():

    scale = []
    amplitude = 1.0
    extraParameter = []

    # model 1
    atom_ex = ot.IsotropicCovarianceModel(ot.MaternModel(), 2)
    atom_ex.setScale([5])
    atom_ex.setAmplitude([1.5])
    scale.append(5)
    amplitude *= 1.5
    extraParameter.append(atom_ex.getKernel().getFullParameter()[-1])

    # model2
    m = ot.MaternModel()
    m.setNu(2.5)
    m.setScale([3])
    m.setAmplitude([3])
    scale.append(3)
    amplitude *= 3
    extraParameter.append(m.getNu())

    # model 3
    atom = ot.IsotropicCovarianceModel(ot.AbsoluteExponential(), 2)
    atom.setScale([2])
    atom.setAmplitude([2.5])
    scale.append(2)
    amplitude *= 2.5

    model = ot.ProductCovarianceModel([atom_ex, m, atom])

    ott.assert_almost_equal(model.getScale(), scale, 1e-16, 1e-16)
    ott.assert_almost_equal(model.getAmplitude(), [amplitude], 1e-16, 1e-16)
    ott.assert_almost_equal(
        model.getFullParameter(), scale + [amplitude] + extraParameter, 1e-16, 1e-16
    )

    # active parameter should be scale + amplitude
    ott.assert_almost_equal(model.getActiveParameter(), [0, 1, 2, 3], 1e-16, 1e-16)

    # setting new parameters
    extraParameter = [2.5, 0.5]
    model.setFullParameter([6, 7, 8, 2] + extraParameter)

    ott.assert_almost_equal(model.getCollection()[0].getScale()[0], 6, 1e-16, 1e-16)
    ott.assert_almost_equal(model.getCollection()[1].getScale()[0], 7, 1e-16, 1e-16)
    ott.assert_almost_equal(model.getCollection()[2].getScale()[0], 8, 1e-16, 1e-16)
    ott.assert_almost_equal(model.getAmplitude()[0], 2, 1e-16, 1e-16)
    ott.assert_almost_equal(
        model.getCollection()[0].getFullParameter()[-1], extraParameter[0], 1e-16, 1e-16
    )
    ott.assert_almost_equal(
        model.getCollection()[1].getFullParameter()[-1], extraParameter[1], 1e-16, 1e-16
    )

    # checking active par setting
    model.setActiveParameter([0, 1, 2, 3, 5])
    ott.assert_almost_equal(
        model.getParameter(), [6, 7, 8, 2, extraParameter[-1]], 1e-16, 1e-16
    )


test_active_parameter()
test_active_amplitude_parameter()
test_parameters_iso()
