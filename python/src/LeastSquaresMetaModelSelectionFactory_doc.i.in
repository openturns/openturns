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
