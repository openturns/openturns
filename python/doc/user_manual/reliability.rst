===========
Reliability
===========

.. currentmodule:: openturns

Events
======

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    StandardEvent
    ProcessEvent
    DomainEvent
    ThresholdEvent
    IntersectionEvent
    UnionEvent

Analytical (FORM/SORM)
======================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

     Analytical
     AnalyticalResult
     FORM
     FORMResult
     MultiFORM
     MultiFORMResult
     SORM
     SORMResult
     SystemFORM
     StrongMaximumTest

Simulation algorithm
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SimulationAlgorithm
    SimulationResult
    EventSimulation
    ProbabilitySimulationResult
    ProbabilitySimulationResultImplementation
    ProbabilitySimulationAlgorithm
    ExpectationSimulationAlgorithm
    ExpectationSimulationResult
    experimental.ActiveLearningReliabilityFunction
    experimental.PythonActiveLearningReliabilityFunction
    experimental.ActiveLearningUFunction
    experimental.ActiveLearningGMMFunction
    experimental.ActiveLearningEFFFunction
    experimental.ActiveLearningReliabilityAlgorithm
    experimental.ActiveLearningReliabilityResult

:class:`ProbabilitySimulationResult` is the result handle returned by the
simulation algorithms: it preserves the dynamic type of the concrete result.
:class:`ProbabilitySimulationResultImplementation` is its implementation class,
which holds the result state and concrete accessors; most users only handle the
former.

Directional Sampling
====================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    DirectionalSampling
    AdaptiveDirectionalStratification
    LineSampling

Root Strategy
-------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    RootStrategy
    RiskyAndFast
    MediumSafe
    SafeAndSlow

Sampling Strategy
-----------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SamplingStrategy
    RandomDirection
    OrthogonalDirection

Importance Sampling
===================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    PostAnalyticalSimulation
    PostAnalyticalControlledImportanceSampling
    PostAnalyticalImportanceSampling
    NAIS
    NAISResult
    CrossEntropyImportanceSampling
    PhysicalSpaceCrossEntropyImportanceSampling
    StandardSpaceCrossEntropyImportanceSampling
    CrossEntropyResult

Subset Sampling
===============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SubsetSampling
    SubsetSamplingResult

Reliability oriented sensitivity analysis
=========================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    SimulationSensitivityAnalysis
