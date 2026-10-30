#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()


def get_points(model):
    """Two distinct points of the model input dimension."""
    dimension = model.getInputDimension()
    s = ot.Point([1.1, 0.6][:dimension])
    t = ot.Point([0.3, -0.2][:dimension])
    return s, t


def check_dimension_errors(model):
    """Every derivative entry point must reject points of wrong dimension."""
    s, t = get_points(model)
    bad = ot.Point(model.getInputDimension() + 1)
    methods = [
        model.partialGradient,
        model.partialHessian,
        model.thirdOrderPartial,
        model.fourthOrderPartial,
        model.parameterGradient,
        model.parameterHessian,
    ]
    for method in methods:
        with ott.assert_raises(Exception):
            method(bad, t)
        with ott.assert_raises(Exception):
            method(s, bad)


def check_partial_hessian(model, rtol=1e-4):
    """partialHessian against a central difference of partialGradient."""
    s, t = get_points(model)
    dimension = model.getInputDimension()
    hessian = model.partialHessian(s, t)
    assert hessian.getDimension() == dimension
    step = 1e-4
    hessian_fd = ot.SymmetricMatrix(dimension)
    for a in range(dimension):
        s_plus = ot.Point(s)
        s_minus = ot.Point(s)
        s_plus[a] += step
        s_minus[a] -= step
        gradient_plus = model.partialGradient(s_plus, t)
        gradient_minus = model.partialGradient(s_minus, t)
        for b in range(dimension):
            hessian_fd[b, a] = (gradient_plus[b, 0] - gradient_minus[b, 0]) / (
                2.0 * step
            )
    ott.assert_almost_equal(
        hessian,
        hessian_fd,
        rtol,
        rtol,
        "in " + model.getClassName() + " partialHessian",
    )


def check_parameter_gradient(model, rtol=1e-4):
    """parameterGradient against a forward difference of the covariance matrix."""
    s, t = get_points(model)
    parameter = ot.Point(model.getParameter())
    gradient = model.parameterGradient(s, t)
    output_dimension = model.getOutputDimension()
    assert gradient.getNbRows() == parameter.getSize()
    assert gradient.getNbColumns() == output_dimension * output_dimension
    model.setParameter(parameter)
    reference = ot.Point(model(s, t).getImplementation())
    step = 1e-6
    for k in range(parameter.getSize()):
        shifted = ot.Point(parameter)
        shifted[k] += step
        model.setParameter(shifted)
        current = ot.Point(model(s, t).getImplementation())
        model.setParameter(parameter)
        for index in range(output_dimension * output_dimension):
            derivative_fd = (current[index] - reference[index]) / step
            ott.assert_almost_equal(
                gradient[k, index],
                derivative_fd,
                rtol,
                rtol,
                "in " + model.getClassName() + " parameterGradient",
            )


def check_parameter_hessian(model, rtol=1e-3, check_values=True):
    """parameterHessian against a forward difference of partialHessian."""
    s, t = get_points(model)
    dimension = model.getInputDimension()
    parameter = ot.Point(model.getParameter())
    parameter_hessian = model.parameterHessian(s, t)
    assert parameter_hessian.getNbRows() == parameter.getSize()
    assert parameter_hessian.getNbColumns() == dimension * dimension
    if not check_values:
        # Composite models evaluate partialHessian through nested finite
        # differences: only its shape is meaningful here.
        return
    model.setParameter(parameter)
    hessian_reference = model.partialHessian(s, t)
    step = 1e-6
    for k in range(parameter.getSize()):
        shifted = ot.Point(parameter)
        shifted[k] += step
        model.setParameter(shifted)
        hessian_shifted = model.partialHessian(s, t)
        model.setParameter(parameter)
        for b in range(dimension):
            for a in range(dimension):
                derivative_fd = (hessian_shifted[a, b] - hessian_reference[a, b]) / step
                ott.assert_almost_equal(
                    parameter_hessian[k, a + dimension * b],
                    derivative_fd,
                    rtol,
                    rtol,
                    "in " + model.getClassName() + " parameterHessian",
                )


def check_high_order_shapes(model):
    """Exercise the generic third/fourth-order paths on every model."""
    s, t = get_points(model)
    dimension = model.getInputDimension()
    third = model.thirdOrderPartial(s, t)
    assert third.getSize() == dimension**3
    fourth = model.fourthOrderPartial(s, t)
    assert fourth.getSize() == dimension**4
    # The (0, 0) component only: every entry must be finite
    for value in third:
        assert value == value
    for value in fourth:
        assert value == value


def check_zero_separation(model, has_parameter_gradient=True):
    """Zero-separation and nugget branches of the analytic implementations."""
    s, _ = get_points(model)
    dimension = model.getInputDimension()
    hessian = model.partialHessian(s, s)
    assert hessian.getDimension() == dimension
    if model.getClassName() == "ExponentialModel":
        # The exponential kernel is not differentiable at zero separation
        with ott.assert_raises(Exception):
            model.partialGradient(s, s)
    else:
        gradient = model.partialGradient(s, s)
        assert gradient.getNbRows() == dimension
        assert gradient.getNbColumns() == model.getOutputDimension() ** 2
    assert model.thirdOrderPartial(s, s).getSize() == dimension**3
    assert model.fourthOrderPartial(s, s).getSize() == dimension**4
    assert model.parameterHessian(s, s).getNbColumns() == dimension * dimension
    if has_parameter_gradient:
        model.setNuggetFactor(0.1)
        gradient = model.parameterGradient(s, s)
        assert gradient.getNbRows() == model.getParameter().getSize()
        model.setNuggetFactor(0.0)


def check_high_order_values():
    """Compare third/fourth derivatives to finite differences on a smooth model."""
    model = ot.SquaredExponential([1.5, 0.8], [2.0])
    s, t = get_points(model)
    dimension = model.getInputDimension()
    # Third-order derivatives: central difference of the analytic Hessian
    step = 1e-3
    third = model.thirdOrderPartial(s, t)
    third_fd = ot.Point(dimension**3)
    for c in range(dimension):
        s_plus = ot.Point(s)
        s_minus = ot.Point(s)
        s_plus[c] += step
        s_minus[c] -= step
        hessian_plus = model.partialHessian(s_plus, t)
        hessian_minus = model.partialHessian(s_minus, t)
        for b in range(dimension):
            for a in range(dimension):
                third_fd[a + dimension * (b + dimension * c)] = (
                    hessian_plus[a, b] - hessian_minus[a, b]
                ) / (2.0 * step)
    ott.assert_almost_equal(third, third_fd, 1e-2, 1e-2, "in SquaredExponential third")
    # Fourth-order derivatives: central difference of the third-order ones
    fourth = model.fourthOrderPartial(s, t)
    fourth_fd = ot.Point(dimension**4)
    for e in range(dimension):
        s_plus = ot.Point(s)
        s_minus = ot.Point(s)
        s_plus[e] += step
        s_minus[e] -= step
        third_plus = model.thirdOrderPartial(s_plus, t)
        third_minus = model.thirdOrderPartial(s_minus, t)
        for c in range(dimension):
            for b in range(dimension):
                for a in range(dimension):
                    fourth_fd[a + dimension * (b + dimension * (c + dimension * e))] = (
                        third_plus[a + dimension * (b + dimension * c)]
                        - third_minus[a + dimension * (b + dimension * c)]
                    ) / (2.0 * step)
    ott.assert_almost_equal(
        fourth, fourth_fd, 1e-1, 1e-1, "in SquaredExponential fourth"
    )


def check_nonstationary_composite_models():
    """Non-stationary CurlFreeModel/DivFreeModel fall back to the base class."""
    basis2d = ot.Basis(
        [
            ot.SymbolicFunction(["x", "y"], ["x"]),
            ot.SymbolicFunction(["x", "y"], ["y"]),
            ot.SymbolicFunction(["x", "y"], ["x*y"]),
        ]
    )
    nonstationary = ot.RankMCovarianceModel([1.0, 2.0, 1.5], basis2d)
    for model in (
        otexp.CurlFreeModel(nonstationary),
        otexp.DivFreeModel(nonstationary),
    ):
        assert not model.isStationary()
        check_dimension_errors(model)
        check_high_order_shapes(model)
        check_zero_separation(model)
        s, t = get_points(model)
        assert model.partialGradient(s, t).getNbColumns() == 4
        assert model.parameterGradient(s, t).getNbColumns() == 4
        assert model.partialHessian(s, t).getDimension() == 2


def check_latent_variable_model():
    """Discrete-level model: no spatial derivative, analytic parameter gradient."""
    model = ot.LatentVariableModel(3, 2)
    level1 = ot.Point([1.0])
    level2 = ot.Point([2.0])
    level0 = ot.Point([0.0])
    # The inputs are discrete levels: no spatial gradient or Hessian
    with ott.assert_raises(Exception):
        model.partialGradient(level1, level2)
    with ott.assert_raises(Exception):
        model.partialHessian(level1, level2)
    parameter = ot.Point(model.getParameter())
    for z1, z2 in ((level1, level2), (level0, level2), (level1, level1)):
        gradient = model.parameterGradient(z1, z2)
        assert gradient.getNbRows() == parameter.getSize()
        assert gradient.getNbColumns() == 1
        model.setParameter(parameter)
        reference = ot.Point(model(z1, z2).getImplementation())
        step = 1e-6
        for k in range(parameter.getSize()):
            shifted = ot.Point(parameter)
            shifted[k] += step
            model.setParameter(shifted)
            current = ot.Point(model(z1, z2).getImplementation())
            model.setParameter(parameter)
            ott.assert_almost_equal(
                gradient[k, 0],
                (current[0] - reference[0]) / step,
                1e-4,
                1e-4,
                "in LatentVariableModel parameterGradient",
            )
    # Invalid levels and dimensions
    with ott.assert_raises(Exception):
        model.parameterGradient(ot.Point([0.5]), level2)
    with ott.assert_raises(Exception):
        model.parameterGradient(ot.Point([-1.0]), level2)
    with ott.assert_raises(Exception):
        model.parameterGradient(level1, ot.Point([3.0]))
    with ott.assert_raises(Exception):
        model.parameterGradient(ot.Point([1.0, 0.0]), level2)
    with ott.assert_raises(Exception):
        model.parameterGradient(level1, ot.Point([1.0, 0.0]))


def check_interface_forwarders():
    """The CovarianceModel interface must forward the derivative methods."""
    implementation = ot.SquaredExponential([1.5, 0.8], [2.0])
    model = ot.CovarianceModel(implementation)
    s, t = get_points(model)
    for name in (
        "partialGradient",
        "partialHessian",
        "parameterGradient",
        "thirdOrderPartial",
        "fourthOrderPartial",
        "parameterHessian",
    ):
        ott.assert_almost_equal(
            getattr(model, name)(s, t),
            getattr(implementation, name)(s, t),
            0.0,
            0.0,
            "in CovarianceModel " + name,
        )


# Models covering every covariance model class with an analytic derivative
spatial_correlation = ot.CorrelationMatrix(2)
spatial_correlation[1, 0] = 0.3
spatial_covariance = ot.CovarianceMatrix(2)
spatial_covariance[0, 0] = 4.0
spatial_covariance[1, 1] = 9.0
spatial_covariance[1, 0] = 2.0
rho = ot.GeneralizedExponential([1.2], [1.0], 1.5)
basis = ot.Basis(
    [ot.SymbolicFunction(["x"], ["x"]), ot.SymbolicFunction(["x"], ["x^2"])]
)
functional = ot.StationaryFunctionalCovarianceModel(
    [1.2], [2.0], ot.SymbolicFunction(["tau"], ["exp(-abs(tau))"])
)
rank_covariance = ot.CovarianceMatrix(2)
rank_covariance[0, 0] = 1.0
rank_covariance[1, 1] = 4.0
rank_covariance[1, 0] = 1.0

# (model, check_parameter_gradient, check_parameter_hessian_values)
# RankMCovarianceModel inherits the finite-difference parameterGradient, which
# needs computeAsScalar(iterator), not implemented by that class.
# Composite models evaluate partialHessian through nested finite differences:
# only the shape of their parameterHessian is checked.
models = [
    (ot.SquaredExponential([1.5, 0.8], [2.0]), True, True),
    (ot.AbsoluteExponential([1.5, 0.8], [2.0]), True, True),
    (ot.GeneralizedExponential([1.5, 0.8], [2.0], 1.0), True, True),
    (ot.GeneralizedExponential([1.5, 0.8], [2.0], 1.5), True, True),
    (ot.GeneralizedExponential([1.5, 0.8], [2.0], 2.0), True, True),
    (ot.MaternModel([1.5, 0.8], [2.0], 0.25), True, True),
    (ot.MaternModel([1.5, 0.8], [2.0], 0.5), True, True),
    (ot.MaternModel([1.5, 0.8], [2.0], 1.5), True, True),
    (ot.MaternModel([1.5, 0.8], [2.0], 2.5), True, True),
    (ot.MaternModel([1.5, 0.8], [2.0], 3.7), True, True),
    (ot.ExponentiallyDampedCosineModel([1.5, 0.8], [2.0], 0.7), True, True),
    (ot.SphericalModel([1.5, 0.8], [2.0], 4.0), True, True),
    (ot.ExponentialModel([1.5, 0.8], spatial_covariance), True, True),
    (ot.ExponentialModel([1.5, 0.8], [2.0]), True, True),
    (ot.KroneckerCovarianceModel(rho, [1.0, 2.0], spatial_correlation), True, True),
    (ot.KroneckerCovarianceModel(rho, [2.0]), True, True),
    (ot.RankMCovarianceModel([1.0, 2.0], basis), False, True),
    (ot.RankMCovarianceModel(rank_covariance, basis), False, True),
    (ot.DiracCovarianceModel(2, [1.0, 2.0], spatial_correlation), True, True),
    (ot.DiracCovarianceModel(2, [2.0]), True, True),
    (ot.FractionalBrownianMotionModel([1.5], [2.0], [0.25]), True, True),
    (ot.FractionalBrownianMotionModel([1.5], [2.0], [0.5]), True, True),
    (functional, True, True),
    (
        ot.ProductCovarianceModel(
            [
                ot.AbsoluteExponential([1.5], [2.0]),
                ot.SquaredExponential([0.8], [1.5]),
            ]
        ),
        True,
        True,
    ),
    (
        # The wrapper scale must match the first model scale, otherwise
        # setScale() resets the first model scale and the parameter
        # finite differences are meaningless.
        ot.TensorizedCovarianceModel(
            [
                ot.SquaredExponential([1.5], [2.0]),
                ot.AbsoluteExponential([0.8], [1.5]),
            ],
            [1.5],
        ),
        True,
        True,
    ),
    (ot.IsotropicCovarianceModel(ot.MaternModel([1.2], [2.0], 1.5), 2), True, True),
    (otexp.CurlFreeModel(ot.SquaredExponential([1.5, 0.8], [2.0])), True, False),
    (otexp.DivFreeModel(ot.SquaredExponential([1.5, 0.8], [2.0])), True, False),
]

for model, has_parameter_gradient, check_parameter_hessian_values in models:
    full_parameter = model.getFullParameter()
    assert model.getFullParameterDescription().getSize() == full_parameter.getSize()
    check_dimension_errors(model)
    check_partial_hessian(model)
    check_parameter_hessian(model, check_values=check_parameter_hessian_values)
    check_high_order_shapes(model)
    check_zero_separation(model, has_parameter_gradient)
    if has_parameter_gradient:
        check_parameter_gradient(model)

check_high_order_values()
check_nonstationary_composite_models()
check_latent_variable_model()
check_interface_forwarders()

print("Covariance model derivative coverage: PASS")
