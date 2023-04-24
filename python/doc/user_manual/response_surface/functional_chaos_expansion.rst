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
    experimental.LeastSquaresExpansion
    experimental.IntegrationExpansion

Construction of the truncated multivariate orthogonal basis
===========================================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    AdaptiveStrategy
    CleaningStrategy
    FixedStrategy

Computation of the polynomial chaos coefficients
================================================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ProjectionStrategy
    IntegrationStrategy
    LeastSquaresStrategy
    ApproximationAlgorithm
    PenalizedLeastSquaresAlgorithmFactory
    PenalizedLeastSquaresAlgorithm
    LeastSquaresMetaModelSelectionFactory
    LeastSquaresMetaModelSelection
    BasisSequenceFactory

    :template: classWithPlot.rst_t
    LARS

    :template: class.rst_t
    FittingAlgorithm
    CorrectedLeaveOneOut
    KFold
    DesignProxy

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

Results
=======

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FunctionalChaosRandomVector
    FunctionalChaosResult
    FunctionalChaosSobolIndices

Functional chaos on fields
==========================

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    experimental.FieldToPointFunctionalChaosAlgorithm
    experimental.FieldFunctionalChaosResult
    experimental.FieldFunctionalChaosSobolIndices
