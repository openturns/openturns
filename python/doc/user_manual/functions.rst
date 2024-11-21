=========
Functions
=========

.. currentmodule:: openturns

General mathematical functions
==============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Function
    FunctionImplementation
    SymbolicFunction
    TranslationFunction

    :template: classWithPlot.rst_t
    LinearFunction
    QuadraticFunction

    :template: class.rst_t
    DatabaseFunction
    PythonFunction
    OpenTURNSPythonFunction
    MemoizeFunction

Algebra of functions
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    AggregatedFunction

    :template: classWithPlot.rst_t
    ComposedFunction

    :template: class.rst_t
    DistanceToDomainFunction
    DualLinearCombinationFunction
    IdentityFunction

    :template: classWithPlot.rst_t
    IndicatorFunction
    LinearCombinationFunction

    :template: class.rst_t
    ParametricFunction

    :template: classWithPlot.rst_t
    ProductFunction

    :template: class.rst_t
    PointToPointEvaluation
    FieldToFieldConnection
    FieldToPointConnection
    PointToFieldConnection
    PointToPointConnection

Field and mixed functions
=========================

Refer to :ref:`field_function`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FieldFunction
    FieldToPointFunction
    PointToFieldFunction
    ParametricPointToFieldFunction
    VertexValuePointToFieldFunction
    ValueFunction
    VertexValueFunction
    PythonFieldFunction
    PythonFieldToPointFunction
    PythonPointToFieldFunction
    OpenTURNSPythonFieldFunction
    OpenTURNSPythonFieldToPointFunction
    OpenTURNSPythonPointToFieldFunction

Collection of functions
=======================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Basis
    OrthogonalBasis
    BasisSequence

Finite differentiation schemes
==============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    CenteredFiniteDifferenceGradient
    CenteredFiniteDifferenceHessian
    FiniteDifferenceGradient
    FiniteDifferenceHessian
    NonCenteredFiniteDifferenceGradient
    FiniteDifferenceStep
    ConstantStep
    BlendedStep

Evaluation functions
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Evaluation
    EvaluationImplementation
    AggregatedEvaluation
    ComposedEvaluation
    DatabaseEvaluation
    DistanceToDomainEvaluation
    DualLinearCombinationEvaluation
    IndicatorEvaluation
    LinearCombinationEvaluation
    LinearEvaluation
    MarginalEvaluation
    MemoizeEvaluation
    NoEvaluation
    ParametricEvaluation

    :template: classWithPlot.rst_t
    PiecewiseHermiteEvaluation
    PiecewiseLinearEvaluation
    P1LagrangeInterpolation

    :template: classWithPlot.rst_t
    P1LagrangeEvaluation
    SymbolicEvaluation

    :template: class.rst_t
    ProductEvaluation
    ProductPolynomialEvaluation
    QuadraticEvaluation
    SymbolicEvaluation
    TranslationEvaluation

Gradient functions
==================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    GradientImplementation
    SymbolicGradient
    ComposedGradient
    ConstantGradient
    DualLinearCombinationGradient
    LinearCombinationGradient
    LinearGradient
    NoGradient
    ParametricGradient
    ProductGradient
    MarginalGradient

Hessian functions
=================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    HessianImplementation
    SymbolicHessian
    ComposedHessian
    ConstantHessian
    DualLinearCombinationHessian
    LinearCombinationHessian
    NoHessian
    NullHessian
    ParametricHessian
    ProductHessian
    MarginalHessian

Differential equation solvers
=============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ODESolver

    :template: ODESolver.rst_t
    RungeKutta
    Fehlberg

Special functions
=================

Special functions are implemented as C++ static methods and are available
as Python functions.

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    SpecFunc.AccurateSum
    SpecFunc.BesselI0
    SpecFunc.BesselI1
    SpecFunc.BesselK
    SpecFunc.BesselKDerivative
    SpecFunc.Beta
    SpecFunc.BitCount
    SpecFunc.Clip01
    SpecFunc.Dawson
    SpecFunc.Debye
    SpecFunc.DeltaLogBesselI10
    SpecFunc.DiGamma
    SpecFunc.DiGammaInv
    SpecFunc.DiLog
    SpecFunc.Ei
    SpecFunc.Erf
    SpecFunc.ErfC
    SpecFunc.ErfCX
    SpecFunc.ErfI
    SpecFunc.ErfInverse
    SpecFunc.Expm1
    SpecFunc.Factorial
    SpecFunc.Faddeeva
    SpecFunc.FaddeevaIm
    SpecFunc.Gamma
    SpecFunc.GammaCorrection
    SpecFunc.HyperGeom_1_1
    SpecFunc.HyperGeom_2_1
    SpecFunc.HyperGeom_2_2
    SpecFunc.IGamma1pm1
    SpecFunc.IncompleteBeta
    SpecFunc.IncompleteBetaInverse
    SpecFunc.IncompleteGamma
    SpecFunc.IncompleteGammaInverse
    SpecFunc.IPow
    SpecFunc.IRoot
    SpecFunc.IsNormal
    SpecFunc.LambertW
    SpecFunc.Log1MExp
    SpecFunc.Log1p
    SpecFunc.Log2
    SpecFunc.LogBesselI0
    SpecFunc.LogBesselI1
    SpecFunc.LogBesselK
    SpecFunc.LogBeta
    SpecFunc.LogBinomialCoefficient
    SpecFunc.LogFactorial
    SpecFunc.LogGamma
    SpecFunc.LogGamma1p
    SpecFunc.NextPowerOfTwo
    SpecFunc.Psi
    SpecFunc.RegularizedIncompleteBeta
    SpecFunc.RegularizedIncompleteBetaInverse
    SpecFunc.RegularizedIncompleteGamma
    SpecFunc.RegularizedIncompleteGammaInverse
    SpecFunc.Stirlerr
    SpecFunc.TriGamma

.. _coupling_tools:


Special constants
=================

- SpecFunc.Infinity : inf
- SpecFunc.MaxScalar : maximum float value
- SpecFunc.LogMaxScalar : log of SpecFunc.MaxScalar
- SpecFunc.MinScalar : minimum positive float value
- SpecFunc.LowestScalar : -inf
- SpecFunc.LogMinScalar : log of SpecFunc.MinScalar


External code coupling
======================

These methods are dedicated to coupling external codes by means of text files.

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    coupling_tools.replace
    coupling_tools.execute
    coupling_tools.get_value
    coupling_tools.get_line_col
    coupling_tools.get_regex
    coupling_tools.get

1-D functions
=============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    UniVariateFunction

.. _testing_module:

Testing module
==============
This module is dedicated to the development in the library.

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    testing.assert_almost_equal

    :template: class.rst_t

    testing.DistributionValidation
