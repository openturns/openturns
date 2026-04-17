%feature("docstring") OT::FunctionalChaosAlgorithm
R"RAW(Functional chaos algorithm.

Refer to :ref:`functional_chaos` to get more details on functional chaos expansion.

Available constructors:
    FunctionalChaosAlgorithm(*inputSample, outputSample*)

    FunctionalChaosAlgorithm(*inputSample, outputSample, distribution*)

    FunctionalChaosAlgorithm(*inputSample, outputSample, distribution, adaptiveStrategy*)

    FunctionalChaosAlgorithm(*inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy*)

    FunctionalChaosAlgorithm(*inputSample, weights, outputSample, distribution, adaptiveStrategy*)

    FunctionalChaosAlgorithm(*inputSample, weights, outputSample, distribution, adaptiveStrategy, projectionStrategy*)

Parameters
----------
inputSample: 2-d sequence of float
    Sample of the input random vectors with size :math:`\sampleSize` and dimension :math:`\inputDim`.
outputSample: 2-d sequence of float
    Sample of the output random vectors with size :math:`\sampleSize` and dimension :math:`\outputDim`.
distribution : :class:`~openturns.Distribution`
    Distribution of the random vector :math:`\inputRV` of dimension :math:`\inputDim`.
    When the *distribution* is unspecified, the
    :meth:`~openturns.MetaModelAlgorithm.BuildDistribution` static
    method is evaluated on the *inputSample*.
adaptiveStrategy : :class:`~openturns.AdaptiveStrategy`
    Strategy of selection of the different terms of the multivariate basis.
projectionStrategy : :class:`~openturns.ProjectionStrategy`
    Strategy of evaluation of the coefficients :math:`a_k`
weights : sequence of float
    Weights :math:`(w_i)_{i = 1, ..., \sampleSize}` associated to the output
    sample, where :math:`\sampleSize` is the sample size.
    Default values are :math:`w_i = \frac{1}{\sampleSize}` for :math:`i = 1, ..., \sampleSize`. 

See also
--------
FunctionalChaosResult

Notes
-----
This class creates a functional chaos expansion  based on an input and output
sample of the model.

Refer to :ref:`functional_chaos` to get more details on functional chaos expansion.

Once the expansion is computed, the :class:`~openturns.FunctionalChaosRandomVector`
class provides methods to get the mean and variance of the PCE.
Moreover, if the input distribution has independent marginals and if the domination method is not used,
the :class:`~openturns.FunctionalChaosSobolIndices` provides Sobol' indices
of the input components.

In order to use the domination method, use the :meth:`setUseDomination` method. 


**Default settings for the adaptive strategy**

When the *adaptiveStrategy* is unspecified, the following :class:`~openturns.FixedStrategy` is selected as an
:class:`~openturns.AdaptiveStrategy`.
The associated basis is built as the tensorization of the univariate polynomials family
orthonormal to the standard representative of the input marginals distribution (if not specified, the input 
distribution is fitted on the input sample).
The basis which is associated to the enumerate function is chosen according to
the `FunctionalChaosAlgorithm-QNorm` parameter of the :class:`~openturns.ResourceMap`:

- If this parameter is equal to 1, then the :class:`~openturns.LinearEnumerateFunction` class is used.
- Otherwise, the :class:`~openturns.HyperbolicAnisotropicEnumerateFunction` class is used.

The first elements of the basis are used to build the approximation space. By default, the number of elements is
defined by the keys `FunctionalChaosAlgorithm-BasisSize` and  `FunctionalChaosAlgorithm-TotalDegree`:

- is the BasisSize is 0, then the number is computed from the total degree (using the enumerate function of the basis);
- is the BasisSize is less than the input sample size, this value is used;
- is the BasisSize is greater than the input sample size, the number is the input sample size.

If the `FunctionalChaosAlgorithm-BasisSize` key of the :class:`~openturns.ResourceMap` is nonzero,
then this parameter sets the basis size.
Otherwise, the `FunctionalChaosAlgorithm-MaximumTotalDegree` key of the
:class:`~openturns.ResourceMap` is used to compute the basis size using the `getBasisSizeFromTotalDegree`
method of the orthogonal basis (with a maximum of :math:`n` terms due to the sample size).
Finally, the :class:`~openturns.FixedStrategy` class is used.

**Default settings for the projection strategy**

When the *projectionStrategy* is unspecified, the 
`FunctionalChaosAlgorithm-Sparse` key of the :class:`~openturns.ResourceMap` is used.
If it is false, then the :class:`~openturns.LeastSquaresStrategy` class is
used, which produces a full PCE, without model selection.
Otherwise, a :class:`~openturns.LARS` PCE is created, i.e.
a sparse PCE is computed using model selection.
In this case, the `FunctionalChaosAlgorithm-FittingAlgorithm`
key of the :class:`~openturns.ResourceMap` is used.

- If this key is equal to 'CorrectedLeaveOneOut', then the
  :class:`~openturns.CorrectedLeaveOneOut` criteria is used.
- If this key is equal to 'KFold', then the
  :class:`~openturns.KFold` criteria is used.
- Otherwise, an exception is produced.

Examples
--------
Create the model:

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> inputDimension = 1
>>> model = ot.SymbolicFunction(['x'], ['x * sin(x)'])
>>> distribution = ot.JointDistribution([ot.Uniform()] * inputDimension)

Build the multivariate orthonormal basis:

>>> polyColl = [0.0] * inputDimension
>>> for i in range(distribution.getDimension()):
...     polyColl[i] = ot.StandardDistributionPolynomialFactory(distribution.getMarginal(i))
>>> enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
>>> productBasis = ot.OrthogonalProductPolynomialFactory(polyColl, enumerateFunction)

Define the  strategy to truncate the multivariate orthonormal basis:
We choose all the polynomials of degree lower or equal to 4.

>>> degree = 4
>>> indexMax = enumerateFunction.getBasisSizeFromTotalDegree(degree)
>>> print(indexMax)
5

We keep all the polynomials of degree lower or equal to 4 (which corresponds
to the 5 first ones):

>>> adaptiveStrategy = ot.FixedStrategy(productBasis, indexMax)

Define the evaluation strategy of the  coefficients:

>>> samplingSize = 50
>>> experiment = ot.MonteCarloExperiment(distribution, samplingSize)
>>> inputSample = experiment.generate()
>>> outputSample = model(inputSample)
>>> projectionStrategy = ot.LeastSquaresStrategy()

Create the chaos algorithm:

>>> algo = ot.FunctionalChaosAlgorithm(
...     inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy
... )
>>> algo.run()

Get the result:

>>> functionalChaosResult = algo.getResult()
>>> # print(functionalChaosResult)  # Pretty-print
>>> metamodel = functionalChaosResult.getMetaModel()

Test it:

>>> X = [0.5]
>>> print(model(X))
[0.239713]
>>> print(metamodel(X))
[0.239514]

There are several methods to define the algorithm: default 
settings are used when the information is not provided by the user.
The simplest is to set only the input and output samples.
In this case, the distribution and its parameters are estimated from
the *inputSample* using the :class:`~openturns.MetaModelAlgorithm.BuildDistribution` class.
See the :doc:`/auto_surrogate_modeling/polynomial_chaos/plot_chaos_build_distribution`
example for more details on this topic.

>>> algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample)

In many cases, the distribution is known and it is best to use this information
when we have it.

>>> algo = ot.FunctionalChaosAlgorithm(inputSample, outputSample, distribution)

A more involved method is to define the method to set the orthogonal basis of functions or
polynomials.
We use the :class:`~openturns.OrthogonalProductPolynomialFactory` class to define the
orthogonal basis of polynomials.
Then we use the :class:`FixedStrategy` to define the maximum number of candidate polynomials
to consider in the expansion, up to the total degree equal to 10.

>>> enumerateFunction = ot.LinearEnumerateFunction(inputDimension)
>>> maximumTotalDegree = 10
>>> totalSize = enumerateFunction.getBasisSizeFromTotalDegree(maximumTotalDegree)
>>> polynomialsList = []
>>> for i in range(inputDimension):
...     marginalDistribution = distribution.getMarginal(i)
...     marginalPolynomial = ot.StandardDistributionPolynomialFactory(marginalDistribution)
...     polynomialsList.append(marginalPolynomial)
>>> basis = ot.OrthogonalProductPolynomialFactory(polynomialsList, enumerateFunction)
>>> adaptiveStrategy = ot.FixedStrategy(basis, totalSize)
>>> algo = ot.FunctionalChaosAlgorithm(
...     inputSample, outputSample, distribution, adaptiveStrategy
... )

The most involved method is to define the way to compute the coefficients,
thanks to the :class:`~openturns.ProjectionStrategy`.
In the next example, we use :class:`~openturns.LARS` to create a
sparse PCE.

>>> selection = ot.LeastSquaresMetaModelSelectionFactory(
...     ot.LARS(),
...     ot.CorrectedLeaveOneOut()
... )
>>> projectionStrategy = ot.LeastSquaresStrategy(inputSample, outputSample, selection)
>>> algo = ot.FunctionalChaosAlgorithm( 
...     inputSample, outputSample, distribution, adaptiveStrategy, projectionStrategy
... )
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::getMaximumResidual
"Get the maximum residual.

Returns
-------
residual : float
    Residual value needed in the projection strategy. 

    Default value is :math:`0`."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::getProjectionStrategy
"Get the projection strategy.

Returns
-------
strategy : :class:`~openturns.ProjectionStrategy`
    Projection strategy.

Notes
-----
The projection strategy selects the different terms of the
multivariate basis to define the subset *K*."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::getAdaptiveStrategy
"Get the adaptive strategy.

Returns
-------
adaptiveStrategy : :class:`~openturns.AdaptiveStrategy`
    Strategy of selection of the different terms of the multivariate basis."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::getResult
"Get the results of the metamodel computation.

Returns
-------
result : :class:`~openturns.FunctionalChaosResult`
    Result structure, created by the method :py:meth:`run`."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::run
"Compute the metamodel.

Notes
-----
Evaluates the metamodel and stores all the results in a result structure."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::setMaximumResidual
"Set the maximum residual.

Parameters
----------
residual : float
    Residual value needed in the projection strategy. 

    Default value is :math:`0`."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::setProjectionStrategy
"Set the projection strategy.

Parameters
----------
projectionStrategy : :class:`~openturns.ProjectionStrategy`
    Strategy to estimate the coefficients :math:`a_k`."

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::setUseDomination
"Domination method flag accessor.

Parameters
----------
useDomination : bool
    Whether to use the domination method.

Notes
-----
The domination method consists in using the basis defined in the argument *adaptiveStrategy* (or its default
implementation if not specified). No
isoprobabilistic transformation is used. This basis it is not necessarily orthonormal with respect to
the *inputDistribution*. As a result, the coefficients can be computed with a least squares method only
(select class:`~openturns.LeastSquaresStrategy` for the *projectionStrategy*) and the Sobol' indices will not
be computed.
"

// ---------------------------------------------------------------------

%feature("docstring") OT::FunctionalChaosAlgorithm::getUseDomination
"Domination method flag accessor.

Returns
-------
useDomination : bool
    Whether the domination method is used.

Notes
-----
The domination method consists in using the basis defined in the argument *adaptiveStrategy* (or its default
implementation if not specified). No
isoprobabilistic transformation is used. This basis it is not necessarily orthonormal with respect to
the *inputDistribution*. As a result, the coefficients can be computed with a least squares method only
(select class:`~openturns.LeastSquaresStrategy` for the *projectionStrategy*) and the Sobol' indices will not
be computed.
"
