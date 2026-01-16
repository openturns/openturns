==================
Surrogate modeling
==================

.. currentmodule:: openturns

General classes
===============

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    MetaModelAlgorithm
    MetaModelResult

    :template: classWithPlot.rst_t

    MetaModelValidation

Parametric
==========

Taylor approximation
--------------------

Refer to :ref:`Taylor expansion <taylor_expansion>`.

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LinearTaylor
    QuadraticTaylor

Least squares approximation
---------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LinearLeastSquares
    QuadraticLeastSquares


Linear model algorithm
======================

Main classes
------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LinearModelAlgorithm
    LinearModelResult
    LinearModelStepwiseAlgorithm

Post-processing
---------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    LinearModelAnalysis

    :template: classWithPlot.rst_t

    experimental.LinearModelValidation

Generalized Linear Model algorithm
==================================

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    GeneralLinearModelAlgorithm

    :template: class.rst_t
    GeneralLinearModelResult


Gaussian Process Regression
===========================

Main classes
------------

.. autosummary::
    :toctree: _generated/
    :template: classWithPlot.rst_t

    GaussianProcessRegression
    GaussianProcessFitter

    :template: class.rst_t
    GaussianProcessRegressionResult
    GaussianProcessFitterResult
    experimental.GaussianProcessRegressionCrossValidation
    GaussianProcessConditionalCovariance
    experimental.GaussianProcessRandomVector

Construction of the regression basis
------------------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    BasisFactory
    ConstantBasisFactory
    LinearBasisFactory
    QuadraticBasisFactory


Functional chaos expansion
==========================

Main classes
------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FunctionalChaosAlgorithm
    LeastSquaresExpansion
    IntegrationExpansion

Construction of the truncated multivariate orthogonal basis
-----------------------------------------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    AdaptiveStrategy
    CleaningStrategy
    FixedStrategy

Projection method
-----------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    ProjectionStrategy
    IntegrationStrategy
    LeastSquaresStrategy

Least squares algorithms to compute the coefficients
----------------------------------------------------

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
-------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    BasisSequenceFactory

    :template: classWithPlot.rst_t
    LARS

Model selection criteria
------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FittingAlgorithm
    CorrectedLeaveOneOut
    KFold

.. _least_squares_methods:

Least Squares problem resolution
--------------------------------

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
-------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FunctionalChaosRandomVector
    FunctionalChaosResult

    :template: classWithPlot.rst_t

    FunctionalChaosSobolIndices
    FunctionalChaosValidation

Functional chaos on fields
--------------------------

.. autosummary::
    :toctree: _generated/
    :template: class.rst_t

    FieldToPointFunctionalChaosAlgorithm
    FieldFunctionalChaosResult
    FieldFunctionalChaosSobolIndices
    PointToFieldFunctionalChaosAlgorithm
