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
    ExponentialModel
    ExponentiallyDampedCosineModel
    SphericalModel

    :template: classWithPlot.rst_t
    UserDefinedCovarianceModel

    :template: class.rst_t
    StationaryCovarianceModel
    UserDefinedCovarianceModel

    :template: classWithPlot.rst_t
    UserDefinedStationaryCovarianceModel

    :template: class.rst_t
    NonStationaryCovarianceModelFactory
    StationaryCovarianceModelFactory

Spectral information
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SpectralModel
    CauchyModel
    UserDefinedSpectralModel
    SpectralModelFactory
    WelchFactory
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
    :template: Process.rst_t

    FunctionalBasisProcess

Composite process
=================

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

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
