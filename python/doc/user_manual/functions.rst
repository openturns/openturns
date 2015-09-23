=========
Functions
=========

.. currentmodule:: openturns

General mathematical functions
==============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    NumericalMathFunction
    PythonFunction
    OpenTURNSPythonFunction

Various functions
=================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ComposedNumericalMathFunction
    LinearNumericalMathFunction
    ProductNumericalMathFunction
    QuadraticNumericalMathFunction

Dynamical functions
===================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DynamicalFunction
    PythonDynamicalFunction
    SpatialFunction
    TemporalFunction

Collection of functions
=======================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Basis
    FunctionalBasis
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

    NumericalMathEvaluationImplementation
    AggregatedNumericalMathEvaluationImplementation
    AnalyticalNumericalMathEvaluationImplementation
    ComposedNumericalMathEvaluationImplementation
    DatabaseNumericalMathEvaluationImplementation
    DualLinearCombinationEvaluationImplementation
    IndicatorNumericalMathEvaluationImplementation
    LinearCombinationEvaluationImplementation
    LinearNumericalMathEvaluationImplementation
    MarginalTransformationEvaluation
    NoNumericalMathEvaluationImplementation
    ParametricEvaluationImplementation
    PiecewiseHermiteEvaluationImplementation
    PiecewiseLinearEvaluationImplementation
    ProductNumericalMathEvaluationImplementation
    ProductPolynomialEvaluationImplementation
    QuadraticNumericalMathEvaluationImplementation

Gradient functions
==================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    NumericalMathGradientImplementation
    AnalyticalNumericalMathGradientImplementation
    ComposedNumericalMathGradientImplementation
    ConstantNumericalMathGradientImplementation
    DualLinearCombinationGradientImplementation
    LinearCombinationGradientImplementation
    LinearNumericalMathGradientImplementation
    NoNumericalMathGradientImplementation
    ParametricGradientImplementation
    ProductNumericalMathGradientImplementation

Hessian functions
=================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    NumericalMathHessianImplementation
    AnalyticalNumericalMathHessianImplementation
    ComposedNumericalMathHessianImplementation
    ConstantNumericalMathHessianImplementation
    DualLinearCombinationHessianImplementation
    LinearCombinationHessianImplementation
    NoNumericalMathHessianImplementation
    ParametricHessianImplementation
    ProductNumericalMathHessianImplementation

Cross-validation algorithm
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ODESolver
    RungeKutta

Special constants & functions
=============================

Numerical constants
-------------------

.. FAILS: http://trac.openturns.org/ticket/724

    .. autodata:: SpecFunc.EULERSQRT6_PI

    .. autodata:: SpecFunc.EulerConstant

    .. autodata:: SpecFunc.ISQRT2PI

    .. autodata:: SpecFunc.LogMaxNumericalScalar

    .. autodata:: SpecFunc.LogMinNumericalScalar

    .. autodata:: SpecFunc.LOGSQRT2PI

    .. autodata:: SpecFunc.MaxNumericalScalar

    .. autodata:: SpecFunc.MaximumIteration

    .. autodata:: SpecFunc.MinNumericalScalar

    .. autodata:: SpecFunc.NumericalScalarEpsilon

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
    SpecFunc_BitCount
    SpecFunc_Cbrt
    SpecFunc_Dawson
    SpecFunc_Debye
    SpecFunc_DiGamma
    SpecFunc_DiGammaInv
    SpecFunc_DiLog
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
    SpecFunc_LambertW
    SpecFunc_LnBeta
    SpecFunc_LnGamma
    SpecFunc_Log1MExp
    SpecFunc_Log1p
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

External code coupling
======================

.. autosummary::
    :toctree: _generated/
    :template: function.rst_t

    coupling_tools.replace
    coupling_tools.execute
    coupling_tools.get_value
    coupling_tools.get_line_col
    coupling_tools.get_regex
