==================
Stochastic process
==================

.. currentmodule:: openturns

General objects
===============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Process

    :template: classWithPlot.rst_t
    Field
    TimeSeries
    ProcessSample

Temporal information
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    CovarianceModel
    CovarianceModelFactory

    :template: CovarianceModel.rst_t
    AbsoluteExponential
    DiracCovarianceModel
    ExponentialModel
    ExponentiallyDampedCosineModel
    GeneralizedExponential
    MaternModel
    ProductCovarianceModel
    SphericalModel
    SquaredExponential
    TensorizedCovarianceModel

    :template: classWithPlot.rst_t
    UserDefinedCovarianceModel

    :template: class.rst_t
    StationaryCovarianceModel

    :template: classWithPlot.rst_t
    UserDefinedStationaryCovarianceModel
    NonStationaryCovarianceModelFactory
    StationaryCovarianceModelFactory

Spectral information
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SpectralModel

    :template: SpectralModel.rst_t
    CauchyModel

    :template: classWithPlot.rst_t
    UserDefinedSpectralModel
    SpectralModelFactory
    WelchFactory

    :template: class.rst_t
    FilteringWindows

    :template: FilteringWindow.rst_t

    Hanning
    Hamming

Link Temporal - Spectral information
====================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SecondOrderModel
    ExponentialCauchy

Normal process
==============

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    SpectralNormalProcess
    TemporalNormalProcess

    :template: class.rst_t
    ConditionedNormalProcess

Functional basis process
========================

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    FunctionalBasisProcess

Composite process
=================

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

     CompositeProcess

Aggregated process
==================

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    AggregatedProcess

ARMA
====

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    ARMA

    :template: class.rst_t
    ARMACoefficients
    ARMAState

ARMA factory
============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ARMAFactory
    WhittleFactoryState
    WhittleFactory
    ARMALikelihoodFactory
    BoxCoxEvaluationImplementation
    InverseBoxCoxEvaluationImplementation
    TrendEvaluationImplementation
    InverseTrendEvaluationImplementation

RandomWalk
==========

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    RandomWalk

WhiteNoise
==========

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    WhiteNoise

Check hypothesis on time series
===============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DickeyFullerTest

Karhunen Loeve decomposition
============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    KarhunenLoeveAlgorithm

    :template: classWithPlot.rst_t
    KarhunenLoeveP1Algorithm
    KarhunenLoeveP1Factory
    KarhunenLoeveQuadratureAlgorithm
    KarhunenLoeveQuadratureFactory

    :template: class.rst_t
    KarhunenLoeveResult
