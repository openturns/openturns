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
    PythonFieldFunction
    ValueFunction
    VertexValueFunction
    PythonFieldFunction
    PythonFieldToPointFunction
    PythonPointToFieldFunction
    OpenTURNSPythonFieldFunction
    OpenTURNSPythonFieldToPointFunction
    OpenTURNSPythonPointToFieldFunction
    P1LagrangeInterpolation

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

    EvaluationImplementation
    AggregatedEvaluation
    SymbolicEvaluation
    ComposedEvaluation
    DatabaseEvaluation
    DistanceToDomainEvaluation
    DualLinearCombinationEvaluation
    IndicatorEvaluation
    LinearCombinationEvaluation
    LinearEvaluation
    MarginalTransformationEvaluation
    NoEvaluation
    ParametricEvaluation
    PiecewiseHermiteEvaluation
    PiecewiseLinearEvaluation
    P1LagrangeEvaluation
    ProductEvaluation
    ProductPolynomialEvaluation
    QuadraticEvaluation
    MarginalEvaluation

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

Special constants & functions
=============================

Special functions
-----------------
Special functions are implemented as C++ static methods and are hence casted
as Python functions.

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    SpecFunc.AccurateSum
    SpecFunc.BesselI0
    SpecFunc.BesselI1
    SpecFunc.BesselK
    SpecFunc.Beta
    SpecFunc.BinomialCoefficient
    SpecFunc.BitCount
    SpecFunc.Cbrt
    SpecFunc.Dawson
    SpecFunc.Debye
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
    SpecFunc.IsNormal
    SpecFunc.LambertW
    SpecFunc.LnBeta
    SpecFunc.LnGamma
    SpecFunc.Log1MExp
    SpecFunc.Log1p
    SpecFunc.Log2
    SpecFunc.LogBesselI0
    SpecFunc.LogBesselI1
    SpecFunc.LogBesselK
    SpecFunc.LogBeta
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
    TimerCallback

.. _coupling_tools:

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
