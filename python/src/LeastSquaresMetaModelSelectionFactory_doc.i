%feature("docstring") OT::LeastSquaresMetaModelSelectionFactory
"Least squares metamodel selection factory.

Parameters
----------
basisSeqFac : :class:`~openturns.BasisSequenceFactory`
    A basis sequence factory.
fittingAlgo : :class:`~openturns.FittingAlgorithm`, optional
    A fitting algorithm.

See also
--------
ApproximationAlgorithm, PenalizedLeastSquaresAlgorithmFactory

Notes
-----
Implementation of an approximation algorithm implementation factory which builds
an :class:`~openturns.ApproximationAlgorithm`: this latter class is not directly usable because
it is operational only within the :class:`~openturns.FunctionalChaosAlgorithm`.

The following :class:`~openturns.ResourceMap` keys are used:

- ``LeastSquaresMetaModelSelection-ErrorThreshold`` (``Scalar``, default: ``0.0``): threshold on the error of the selection.
- ``LeastSquaresMetaModelSelection-MaximumError`` (``Scalar``, default: ``0.5``): maximum relative error of the selection.
- ``LeastSquaresMetaModelSelection-MaximumErrorFactor`` (``Scalar``, default: ``2.0``): factor applied to the error of the selection.
- ``LeastSquaresMetaModelSelection-DecompositionMethod`` (``String``, default: ``SVD``): name of the decomposition method used to solve the least squares problem. The possible values are ``SVD``, ``Cholesky`` and ``QR``.
- ``LeastSquaresMetaModelSelection-SmallBasisSize`` (``UnsignedInteger``, default: ``10``): size below which the basis is considered small.

Examples
--------
>>> import openturns as ot
>>> basisSequenceFactory = ot.LARS()
>>> fittingAlgorithm = ot.CorrectedLeaveOneOut()
>>> approximationAlgorithm = ot.LeastSquaresMetaModelSelectionFactory(
...                                     basisSequenceFactory, fittingAlgorithm)"

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresMetaModelSelectionFactory::getBasisSequenceFactory
"Accessor to the basis sequence factory.

Returns
-------
basis : :class:`~openturns.BasisSequenceFactory`
    Basis sequence factory."

// ---------------------------------------------------------------------

%feature("docstring") OT::LeastSquaresMetaModelSelectionFactory::getFittingAlgorithm
"Accessor to the fitting algorithm.

Returns
-------
algo : :class:`~openturns.FittingAlgorithm`
    Fitting algorithm."
