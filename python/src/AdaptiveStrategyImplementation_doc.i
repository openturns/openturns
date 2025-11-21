%define OT_AdaptiveStrategy_doc
R"RAW(Base class for the construction of the truncated multivariate orthogonal basis.

Parameters
----------
orthogonalBasis : :class:`~openturns.OrthogonalBasis`
    An OrthogonalBasis.
dimension : positive int
    Number of terms of the basis. This first usage has the same implementation
    as the second with a :class:`~openturns.FixedStrategy`. 

See also
--------
FunctionalChaosAlgorithm, FixedStrategy, CleaningStrategy

Notes
-----
A strategy must be chosen for the selection of the different terms of the
multivariate basis in which the response surface by functional chaos is expressed.
The selected terms are regrouped in the finite subset :math:`K` of :math:`\Nset`.

There are two different strategies available:

- :class:`~openturns.FixedStrategy`,
- :class:`~openturns.CleaningStrategy`.

These strategies are conceived in such a way to be adapted for other orthogonal
expansions (other than polynomial). For the moment, their implementation are
only useful for the polynomial chaos expansion.)RAW"
%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation
OT_AdaptiveStrategy_doc


// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_computeInitialBasis_doc
"Compute initial basis for the approximation.

See also
--------
getPsi"

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::computeInitialBasis
OT_AdaptiveStrategy_computeInitialBasis_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_getBasis_doc
"Accessor to the underlying orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Orthogonal basis of which the adaptive strategy is based."

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::getBasis
OT_AdaptiveStrategy_getBasis_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_getMaximumDimension_doc
"Accessor to the maximum dimension of the orthogonal basis.

Returns
-------
maximumDimension : int
    Maximum dimension of the truncated basis."

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::getMaximumDimension
OT_AdaptiveStrategy_getMaximumDimension_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_getPsi_doc
"Accessor to the selected orthogonal polynomials in the basis.

The value returned by this method depends on the specific
choice of adaptive strategy and the previous calls to the
:meth:`updateBasis` method.

Returns
-------
polynomials : list of polynomials
    Sequence of :math:`P` polynomials.

Notes
-----
The method :meth:`computeInitialBasis` must be applied first.

Examples
--------
>>> import openturns as ot
>>> productBasis = ot.OrthogonalProductPolynomialFactory([ot.HermiteFactory()])
>>> adaptiveStrategy = ot.FixedStrategy(productBasis, 3)
>>> adaptiveStrategy.computeInitialBasis()
>>> print(adaptiveStrategy.getPsi())
[1,x0,-0.707107 + 0.707107 * x0^2]"

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::getPsi
OT_AdaptiveStrategy_getPsi_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_setMaximumDimension_doc
"Accessor to the maximum dimension of the orthogonal basis.

Parameters
----------
maximumDimension : int
    Maximum dimension of the truncated basis."

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::setMaximumDimension
OT_AdaptiveStrategy_setMaximumDimension_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_updateBasis_doc
"Update the basis for the next iteration of approximation.

Parameters
----------
alpha_k: sequence of floats
    The coefficients of the expansion at this step.
residual: float
    The current value of the residual.
relativeError: float
    The relative error."

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::updateBasis
OT_AdaptiveStrategy_updateBasis_doc

// ---------------------------------------------------------------------

%define OT_AdaptiveStrategy_involvesModelSelection_doc
"Get the model selection flag.

A model selection method can be used to select the coefficients
of the decomposition which enable to best predict the output.
Model selection can lead to a sparse functional chaos expansion.

Returns
-------
involvesModelSelection : bool
    True if the method involves a model selection method."

%enddef
%feature("docstring") OT::AdaptiveStrategyImplementation::involvesModelSelection
OT_AdaptiveStrategy_involvesModelSelection_doc
