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
an :class:`~openturns.ApproximationAlgorithm`.

This class is not usable because it is operational only within the
:class:`~openturns.FunctionalChaosAlgorithm`.

This class uses the following entries of :class:`~openturns.ResourceMap`:

- *LeastSquaresMetaModelSelection-ErrorThreshold* (:raw:`Scalar`, default: `0.0`)
- *LeastSquaresMetaModelSelection-MaximumError* (:raw:`Scalar`, default: `0.5`)
- *LeastSquaresMetaModelSelection-MaximumErrorFactor* (:raw:`Scalar`, default: `2.0`)
- *LeastSquaresMetaModelSelection-DecompositionMethod* (:raw:`String`, default: `SVD`)
- *LeastSquaresMetaModelSelection-SmallBasisSize* (:raw:`UnsignedInteger`, default: `10`)

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
