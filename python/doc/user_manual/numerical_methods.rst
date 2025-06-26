=================
Numerical methods
=================

.. currentmodule:: openturns

Optimization
============

.. _optimization_algorithms:

Refer to :ref:`optimization_algorithm`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    OptimizationProblem
    NearestPointProblem
    LeastSquaresProblem
    OptimizationAlgorithm
    OptimizationResult
    AbdoRackwitz
    Bonmin
    Ceres
    CMinpack
    Cobyla
    Dlib
    experimental.EfficientGlobalOptimization
    experimental.HiGHS
    Ipopt
    MultiStart
    NLopt
    Pagmo
    SQP
    TNC

Integration
===========

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    IntegrationAlgorithm
    FilonQuadrature
    CubaIntegration
    SimplicialCubature
    ExperimentIntegration

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    GaussKronrod
    GaussLegendre
    FejerAlgorithm

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    GaussKronrodRule

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    IteratedQuadrature

Transformations
===============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FFT
    KissFFT

    :template: classWithPlot.rst_t

    BoxCoxFactory
    BoxCoxTransform
    InverseBoxCoxTransform
    TrendFactory

    :template: class.rst_t

    TrendTransform
    InverseTrendTransform

Spatial lookup
==============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    NearestNeighbourAlgorithm
    KDTree
    NaiveNearestNeighbour
    NearestNeighbour1D
    RegularGridNearestNeighbour
    EnclosingSimplexAlgorithm
    NaiveEnclosingSimplex
    RegularGridEnclosingSimplex
    EnclosingSimplexMonotonic1D
    BoundingVolumeHierarchy

Classification
==============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    Classifier

    :template: classWithPlot.rst_t

    MixtureClassifier
    MinimumVolumeClassifier

    :template: class.rst_t

    ExpertMixture

Combinatorial generators
========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    CombinatorialGenerator
    Combinations
    KPermutations
    Tuples

