%feature("docstring") OT::FunctionalChaosResult
R"RAW(Functional chaos result.

Returned by functional chaos algorithms, see :class:`~openturns.FunctionalChaosAlgorithm`.

Refer to :any:`functional_chaos` to get more details on functional chaos expansion.

Parameters
----------
sampleX : 2-d sequence of float
    Input sample of :math:`\inputRV \in \Rset^{\inputDim}`.
sampleY : 2-d sequence of float
    Output sample of :math:`\outputRV \in \Rset^{\outputDim}`.
distribution : :class:`~openturns.Distribution`
    Distribution of the random vector :math:`\inputRV`
transformation : :class:`~openturns.Function`
    The function that maps the input :math:`\inputRV` to the 
    standardized input :math:`\standardRV`.
inverseTransformation : :class:`~openturns.Function`
    The function that maps the standardized input :math:`\standardRV` to
    the input :math:`\inputRV`.
orthogonalBasis : :class:`~openturns.OrthogonalBasis`
    The multivariate orthogonal basis.
indices : sequence of `int`
    The indices of the selected basis function within the orthogonal basis.
alpha_k : 2-d sequence of `float`
    The coefficients of the functional chaos expansion.
Psi_k : sequence of :class:`~openturns.Function`
    The functions of the multivariate basis selected by the algorithm.
isLeastSquares : bool
    True if the expansion is computed using least squares.
isModelSelection : bool
    True if the expansion is computed using model selection.

Notes
-----
Let :math:`\sampleSize \in \Nset` be the sample size.
Let :math:`\outputDim \in \Nset` be the dimension of the output of the model.
For any :math:`j = 1, ..., \sampleSize` and any :math:`i = 1, ..., \outputDim`,
let :math:`y_{j, i} \in \Rset` be the output of the
model and let :math:`\hat{y}_{j, i} \in \Rset` be the output of the meta model.
For any :math:`i = 1, ..., \outputDim`, let :math:`\outputRV_i \in \Rset^\sampleSize` be
the sample output and let :math:`\widehat{\outputRV}_i \in \Rset^\sampleSize` be the
output predicted by the metamodel.
The marginal residual is:

.. math::

    \hat{r}_i = \frac{\sqrt{SS_i}}{\sampleSize} 

for :math:`i = 1, ..., \outputDim`, where :math:`SS_i` is the marginal sum of squares:

.. math::

    SS_i = \sum_{j = 1}^\sampleSize (y_{j, i} - \hat{y}_{j, i})^2.

The marginal relative error is:

.. math::

    \widehat{re}_i = \frac{\hat{r}_i / \sampleSize}{\hat{s}_{Y, i}^2}

for :math:`i = 1, ..., \outputDim`, where :math:`\hat{s}_{Y, i}^2` is the unbiased 
sample variance of the :math:`i`-th output.

This structure is created by the method `run()` of
:class:`~openturns.FunctionalChaosAlgorithm`, and obtained thanks to the 
`getResult()` method.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getCoefficients
R"RAW(Get the coefficients.

Returns
-------
coefficients : 2-d sequence of float
    Coefficients :math:`(\vect{a_k})_{k \in \set{J}^P_s}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getComposedMetaModel
R"RAW(Get the composed metamodel.

The composed metamodel is defined on the standard space :math:`\standardInputSpace`. 
It is defined by the equation:

.. math::
    \tilde{h}(\standardReal) =  \sum_{k \in \set{J}^P_s} \vect{a}_k \psi_k(\standardReal)

for any :math:`\standardReal \in \standardInputSpace`.

Returns
-------
composedMetamodel : :class:`~openturns.Function`
    The metamodel in the standard space :math:`\standardInputSpace`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getMetaModel
R"RAW(Get the metamodel.

Returns
-------
metamodel : :class:`~openturns.Function`
    :math:`\tilde{g} = \sum_{k \in \set{J}^P_s} \vect{a}_k \psi_k \circ T`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getDistribution
R"RAW(Get the input distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    Distribution of the  input random vector :math:`\inputRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getIndices
R"RAW(Get the indices of the final basis.

Returns
-------
indices : :class:`~openturns.Indices`
    Indices :math:`\set{J}^P_s` of the elements of the multivariate basis used in the decomposition.
    Each integer in this list is the input argument of the :class:`~.openturns.EnumerateFunction`.
    If a model selection method such as :class:`~.openturns.LARS` is used,
    these indices are not contiguous.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getInverseTransformation
R"RAW(Get the inverse isoprobabilistic transformation.

Returns
-------
invTransf : :class:`~openturns.Function`
     :math:`T^{-1}` such that :math:`T(\inputRV) = \standardRV`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getOrthogonalBasis
"Get the orthogonal basis.

Returns
-------
basis : :class:`~openturns.OrthogonalBasis`
    Factory of the orthogonal basis."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getReducedBasis
R"RAW(Get the reduced basis.

Returns
-------
basis : list of :class:`~openturns.Function`
    Collection of the functions  :math:`(\psi_k)_{k\in \set{J}^P_s}`  used in the 
    decomposition.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getTransformation
R"RAW(Get the isoprobabilistic transformation.

Returns
-------
transformation : :class:`~openturns.Function`
    Transformation :math:`T` such that :math:`T(\inputRV) = \standardRV`.)RAW"
    
// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::drawSelectionHistory
"Draw the basis selection history.

This is only available with :class:`~openturns.LARS`, and when the output dimension is 1.

Returns
-------
graph : :class:`~openturns.Graph`
    The evolution of the basis coefficients at each selection iteration"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getCoefficientsHistory
"The coefficients values selection history accessor.

This is only available with :class:`~openturns.LARS`, and when the output dimension is 1.

Returns
-------
coefficientsHistory : 2-d sequence of float
    The coefficients values selection history, for each iteration.
    Each inner list gives the coefficients values of the basis terms at i-th iteration."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getIndicesHistory
"The basis indices selection history accessor.

This is only available with :class:`~openturns.LARS`, and when the output dimension is 1.

Returns
-------
indicesHistory : 2-d sequence of int
    The basis indices selection history, for each iteration.
    Each inner list gives the indices of the basis terms at i-th iteration."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::setSelectionHistory
"The basis coefficients and indices accessor.

Parameters
----------
indicesHistory : 2-d sequence of int
    The basis indices selection history
coefficientsHistory : 2-d sequence of float
    The coefficients values selection history
    Must be of same size as indicesHistory."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getConditionalExpectation
R"RAW(Get the conditional expectation of the expansion given one vector input.

This method returns the functional chaos result corresponding to the conditional
expectation of the output given an input vector.

Refer to :any:`functional_chaos` to get details on the mathematical result.

Parameters
----------
conditioningIndices : sequence of int in [0, inputDimension - 1]
    The indices :math:`\vect{u}` of the input random vector to condition.

Returns
-------
conditionalPCE : :class:`~openturns.FunctionalChaosResult`
    The functional chaos result of the conditional expectation.
    Its input dimension is :math:`\operatorname{card}(\vect{u})` and its output dimension
    is :math:`\outputDim` (i.e. the output dimension is unchanged).

Notes
-----
Refer to :any:`functional_chaos` to get details on the mathematical aspects and the notations.

Let :math:`\inputRV \in \Rset^{\inputDim}` be the input random vector distributed according
to :math:`\mu_{\inputRV}`. It is assumed to have independent marginals. We assume that the functional
chaos result has been built within the basis :math:`\left(\psi_k\right)_{k \in I_n}`  which is orthonormal
with respect to  :math:`\mu_{\inputRV}`. This basis is built as the tensorization of univariate basis
orthonormal with respect to  the marginal distributions.

Let :math:`\vect{u} \subseteq \{1, ..., \inputDim\}`
be a set of marginal indices and :math:`\overline{\vect{u}}` the complementary set.
Let :math:`\inputRV_{\vect{u}} \in \Rset^{|\vect{u}|}` be the random vector
corresponding to the group of input variables of index in  :math:`\vect{u}`, 
where :math:`|\vect{u}| = \operatorname{card}(\vect{u})`
is the number of input variables in the group. We denote by :math:`\inputRV_{\overline{\vect{u}}}`
the complementary random vector.
Let :math:`\metaModel(\inputRV)` be the functional chaos
expansion of the physical model :math:`\model`.
This function returns the functional chaos expansion of:

.. math::

    \metaModel_{\vect{u}}\left(\inputReal_{\vect{u}}\right) 
    = \mathbb{E}_{\vect{X}_{\overline{\vect{u}}}} 
    \left[\metaModel(\inputRV) | \inputRV_{\vect{u}} = \inputReal_{\vect{u}}\right]

for any :math:`\inputReal_{\vect{u}} \in \Rset^{|\vect{u}|}`.

An example is provided in :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_chaos_conditional_expectation`.

)RAW"
    
// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::drawErrorHistory
"Draw the error history.

This is only available with :class:`~openturns.LARS`, and when the output dimension is 1.

Returns
-------
graph : :class:`~openturns.Graph`
    The evolution of the error at each selection iteration"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getErrorHistory
"The error history accessor.

This is only available with :class:`~openturns.LARS`, and when the output dimension is 1.

Returns
-------
errorHistory : sequence of float
    The error history"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::setErrorHistory
"The error history accessor.

Parameters
----------
errorHistory : sequence of float
    The error history"

%feature("docstring") OT::FunctionalChaosResult::isLeastSquares
"Get the least squares flag.

Returns
-------
isLeastSquares : bool
    True if the coefficients were estimated from least squares."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::involvesModelSelection
"Get the model selection flag.

A model selection method can be used to select the coefficients
of the decomposition which enable to best predict the output.
Model selection can lead to a sparse functional chaos expansion.

Returns
-------
involvesModelSelection : bool
    True if the method involves a model selection method."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::setIsLeastSquares
"Set the least squares flag.

Parameters
----------
isLeastSquares : bool
    True if the coefficients were estimated from least squares."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::setInvolvesModelSelection
"Set the model selection flag.

A model selection method can be used to select the coefficients
of the decomposition which enable to best predict the output.
Model selection can lead to a sparse functional chaos expansion.

Parameters
----------
involvesModelSelection : bool
    True if the method involves a model selection method."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getSampleResiduals
R"RAW(Get residuals sample.

Returns
-------
residualsSample : :class:`~openturns.Sample`
    The sample of residuals :math:`r_{ji} = y_{ji} - \metaModel_i(\vect{x^{(j)}})`
    for :math:`i = 1, ..., n_Y` and :math:`j = 1, ..., n`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getMarginal
R"RAW(Get the marginal chaos.

When the chaos expansion has several outputs, this method can be used
to extract a particular marginal.
If there are several outputs, zero marginal coefficients may occur: these
are filtered out of the marginal chaos if possible.

Parameters
----------
outputIndex : int or list of ints, :math:`0 \leq i < \outputDim`
    The index or indices of the output marginal.

Returns
-------
marginalPCE : :class:`~openturns.FunctionalChaosResult`
    The marginal chaos.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::setUseDomination
"Domination method flag accessor.

Parameters
----------
useDomination : bool
    Whether to use the domination method.

Notes
-----
The domination method consists in using the basis defined in the argument *adaptiveStrategy*. No
isoprobabilistic transformation is used. This basis it is not necessarily orthonormal with respect to
the *inputDistribution*. As a result, the coefficients can be computed with a least squares method only
(:math:`isLeastSquares` must be *True*) and the Sobol' indices will not be computed.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosResult::getUseDomination
"Domination method flag accessor.

Returns
-------
useDomination : bool
    Whether to use the domination method.

Notes
-----
The domination method consists in using the basis defined in the argument *adaptiveStrategy*. No
isoprobabilistic transformation is used. This basis it is not necessarily orthonormal with respect to
the *inputDistribution*. As a result, the coefficients must have been computed with a least squares method only
(:meth:`isLeastSquares` must be *True*) and the Sobol' indices will not be computed.
"
