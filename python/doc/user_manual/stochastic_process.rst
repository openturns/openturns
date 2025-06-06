==================
Stochastic process
==================

.. currentmodule:: openturns

General objects
===============

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    Process

    :template: classWithPlot.rst_t
    Field
    TimeSeries
    ProcessSample

Temporal information
====================

Refer to :ref:`covariance_model`, :ref:`stationary_covariance_model`.

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
    FractionalBrownianMotionModel
    GeneralizedExponential

    :template: class.rst_t
    IsotropicCovarianceModel
    KroneckerCovarianceModel

    :template: CovarianceModel.rst_t
    MaternModel
    ProductCovarianceModel
    RankMCovarianceModel
    SphericalModel
    SquaredExponential
    TensorizedCovarianceModel
    StationaryFunctionalCovarianceModel

    :template: classWithPlot.rst_t
    UserDefinedCovarianceModel

    :template: classWithPlot.rst_t
    UserDefinedStationaryCovarianceModel
    NonStationaryCovarianceModelFactory
    StationaryCovarianceModelFactory

Categorical variables
=====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    experimental.LatentVariableModel

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

    Hann
    Hamming

Gaussian process
================

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    SpectralGaussianProcess
    GaussianProcess

    :template: class.rst_t
    experimental.ConditionedGaussianProcess

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
    BoxCoxEvaluation
    InverseBoxCoxEvaluation
    TrendEvaluation
    InverseTrendEvaluation

Others
==========

.. autosummary::
    :toctree: _generated/
    :template: Process.rst_t

    DiscreteMarkovChain
    RandomWalk
    WhiteNoise

Check hypothesis on time series
===============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DickeyFullerTest

Karhunen-Loeve decomposition
============================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    KarhunenLoeveAlgorithm

    :template: classWithPlot.rst_t
    KarhunenLoeveP1Algorithm
    KarhunenLoeveQuadratureAlgorithm
    KarhunenLoeveSVDAlgorithm
    KarhunenLoeveReduction
    KarhunenLoeveValidation

    :template: class.rst_t
    KarhunenLoeveResult
    KarhunenLoeveProjection
    KarhunenLoeveLifting
