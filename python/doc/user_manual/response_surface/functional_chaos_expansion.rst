============================================
Response surface: Functional chaos expansion
============================================

.. currentmodule:: openturns

Functional chaos algorithm
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FunctionalChaosAlgorithm
    LeastSquaresExpansion
    IntegrationExpansion

Construction of the truncated multivariate orthogonal basis
===========================================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    AdaptiveStrategy
    CleaningStrategy
    FixedStrategy

Projection method
=================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ProjectionStrategy
    IntegrationStrategy
    LeastSquaresStrategy

Least squares algorithms to compute the coefficients
====================================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ApproximationAlgorithm
    ApproximationAlgorithmImplementationFactory
    PenalizedLeastSquaresAlgorithmFactory
    PenalizedLeastSquaresAlgorithm
    LeastSquaresMetaModelSelectionFactory
    LeastSquaresMetaModelSelection

Model selection algorithm
=========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    BasisSequenceFactory

    :template: classWithPlot.rst_t
    LARS

Model selection criteria
========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FittingAlgorithm
    CorrectedLeaveOneOut
    KFold

.. _least_squares_methods:

Least Squares problem resolution
================================

Refer to :ref:`least_squares`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LeastSquaresMethod
    CholeskyMethod
    SVDMethod
    QRMethod
    SparseMethod
    DesignProxy

Results
=======

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FunctionalChaosRandomVector
    FunctionalChaosResult

    :template: classWithPlot.rst_t

    FunctionalChaosSobolIndices

Functional chaos on fields
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    experimental.FieldToPointFunctionalChaosAlgorithm
    experimental.FieldFunctionalChaosResult
    experimental.FieldFunctionalChaosSobolIndices
    experimental.PointToFieldFunctionalChaosAlgorithm
