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
    ParametricHessian
    ProductHessian

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

Numerical constants
-------------------

.. FAILS: http://trac.openturns.org/ticket/724

    .. autodata:: SpecFunc.EULERSQRT6_PI

    .. autodata:: SpecFunc.EulerConstant

    .. autodata:: SpecFunc.ISQRT2PI

    .. autodata:: SpecFunc.LogMaxScalar

    .. autodata:: SpecFunc.LogMinScalar

    .. autodata:: SpecFunc.LOGSQRT2PI

    .. autodata:: SpecFunc.MaxScalar

    .. autodata:: SpecFunc.MaximumIteration

    .. autodata:: SpecFunc.MinScalar

    .. autodata:: SpecFunc.ScalarEpsilon

    .. autodata:: SpecFunc.PI2_3

    .. autodata:: SpecFunc.PI2_6

    .. autodata:: SpecFunc.PI_SQRT3

    .. autodata:: SpecFunc.PI_SQRT6

    .. autodata:: SpecFunc.Precision

    .. autodata:: SpecFunc.SQRT2PI

    .. autodata:: SpecFunc.SQRT3_PI

    .. autodata:: SpecFunc.ZETA3

Special functions
-----------------

Special functions are implemented as C++ static methods and are hence casted
as Python functions.

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    SpecFunc_BesselI0
    SpecFunc_BesselI1
    SpecFunc_BesselK
    SpecFunc_Beta
    SpecFunc_BinomialCoefficient
    SpecFunc_BitCount
    SpecFunc_Cbrt
    SpecFunc_Dawson
    SpecFunc_Debye
    SpecFunc_DiGamma
    SpecFunc_DiGammaInv
    SpecFunc_DiLog
    SpecFunc_Ei
    SpecFunc_Erf
    SpecFunc_ErfC
    SpecFunc_ErfCX
    SpecFunc_ErfI
    SpecFunc_ErfInverse
    SpecFunc_Expm1
    SpecFunc_Faddeeva
    SpecFunc_FaddeevaIm
    SpecFunc_Gamma
    SpecFunc_GammaCorrection
    SpecFunc_HyperGeom_1_1
    SpecFunc_HyperGeom_2_1
    SpecFunc_HyperGeom_2_2
    SpecFunc_IGamma1pm1
    SpecFunc_IncompleteBeta
    SpecFunc_IncompleteBetaInverse
    SpecFunc_IncompleteGamma
    SpecFunc_IncompleteGammaInverse
    SpecFunc_IsNormal
    SpecFunc_LambertW
    SpecFunc_LnBeta
    SpecFunc_LnGamma
    SpecFunc_Log1MExp
    SpecFunc_Log1p
    SpecFunc_Log2
    SpecFunc_LogBesselI0
    SpecFunc_LogBesselI1
    SpecFunc_LogBesselK
    SpecFunc_LogBeta
    SpecFunc_LogGamma
    SpecFunc_LogGamma1p
    SpecFunc_NextPowerOfTwo
    SpecFunc_Psi
    SpecFunc_RegularizedIncompleteBeta
    SpecFunc_RegularizedIncompleteBetaInverse
    SpecFunc_RegularizedIncompleteGamma
    SpecFunc_RegularizedIncompleteGammaInverse
    SpecFunc_TriGamma
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
