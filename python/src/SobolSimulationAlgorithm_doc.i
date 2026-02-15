%feature("docstring") OT::SobolSimulationAlgorithm
R"RAW(Sobol indices computation using iterative sampling.

The algorithm uses sampling of the distribution of the random
vector :math:`\inputRV` through the model :math:`\model`
to iteratively estimate the Sobol indices.

The algorithm works as follows. 
At each iteration, a new design of experiment with size :math:\sampleSize is 
generated.
This sample is divided into sub-samples, or "blocks".
Each block is evaluated using a single call to the function :math:\model, which 
allows for parallel evaluation.
The distribution of the indices (first and total order) is then computed on this 
current sample.
Finally, the global distribution of the indices is updated.

Using a block size larger than 1 allows the model to evaluate multiple input points
in parallel rather than sequentially.
This can substantially reduce computation time when the model supports vectorized
evaluation, such as when using a :class:`~openturns.PythonFunction` with the
`func_sample` option.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The random vector to study.
model : :class:`~openturns.Function`
    The function to study.
estimator : :class:`~openturns.SobolIndicesAlgorithm`
    The estimator of the indices.

Notes
-----
The algorithm can operate on a multivariate model :math:`\model`, in this case it operates on aggregated indices.

Several estimators are available (Saltelli, Jansen, ...).

Let us denote by :math:`\inputDim` the number of input variables. 
For any :math:`j \in \{ 1, \ldots, \inputDim\}`, let us denote by :math:`\Phi_j^F` (resp. :math:`\Phi_j^T`) the cumulative distribution 
function of the gaussian asymptotic distribution of the estimator 
of the Sobol' first (resp. total) order index.
Let :math:`\alpha\in[0,1]` be the level of the confidence interval and :math:`\epsilon\in(0,1]` the 
length of this confidence interval. 
The algorithms stops when, on all components, first and total order indices have been 
estimated with enough precision.

The precision is said to be sufficient if the length of the :math:`\alpha`-level confidence interval 
is smaller than :math:`\epsilon`:

.. math::

    (\Phi_j^F)^{-1}(1-\alpha/2) - (\Phi_j^F)^{-1}(\alpha/2) \leq \epsilon \textrm{ and } 
    (\Phi_j^T)^{-1}(1-\alpha/2) - (\Phi_j^T)^{-1}(\alpha/2) \leq \epsilon

for all :math:`j \in \{ 1, \ldots, \inputDim\}`.

The total number of samples evaluated by the algorithm is :math:`n_{iter} \times \sampleSize \times (2 + \inputDim)` (no second order indices) where

- :math:`n_{iter}` is the actual number of outer iterations, capped by :meth:`setMaximumOuterSampling` ;
- :math:`\sampleSize` is the internal size of the Sobol' experiment, as set by :meth:`setExperimentSize` ;
- :math:`\inputDim` is the input dimension.

That is why, unlike other simulation algorithms, the total of evaluations is not equal to
the product of iterations (obtained by `getOuterSampling` from the result) by the block size (:meth:`getBlockSize`).

This class makes use of the following :class:`~openturns.ResourceMap` entries:

- `SobolSimulationAlgorithm-DefaultIndexQuantileLevel`: default float value for the confidence level :math:`\alpha`
- `SobolSimulationAlgorithm-DefaultIndexQuantileEpsilon`: default float value for the confidence length :math:`\epsilon`
- `SobolSimulationAlgorithm-DefaultExperimentSize`: default integer value for :math:`N`

See also
--------
SobolSimulationResult, SobolIndicesExperiment, SobolIndicesAlgorithm

Examples
--------
In this example, the experiment size is 200 and the block size is 64.
Hence, a sample with size 200 is first generated.
Then it is split into 3 blocks of size 64 and one block of size 8, 
since :math:`200 = 3 \times 64 + 8`.
This is why the function is evaluated 4 times.
This means that for each iteration, the model is evaluated 4 times:
3 times with a sample of size 64, and once with a sample of size 8.

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> distribution = ot.JointDistribution([ot.Uniform(-1.0, 1.0)] * 3)
>>> model = ot.SymbolicFunction(['x1', 'x2', 'x3'], ['x1 * x2 + x3'])
>>> estimator = ot.SaltelliSensitivityAlgorithm()
>>> estimator.setUseAsymptoticDistribution(True)
>>> algo = ot.SobolSimulationAlgorithm(distribution, model, estimator)
>>> algo.setMaximumOuterSampling(25)  # maximum number of iterations
>>> algo.setExperimentSize(200)  # size of Sobol experiment at each iteration
>>> algo.setBlockSize(64) # number of points evaluated simultaneously
>>> algo.setIndexQuantileLevel(0.05)  # alpha
>>> algo.setIndexQuantileEpsilon(1e-2)  # epsilon
>>> algo.run()
>>> result = algo.getResult()
>>> fo = result.getFirstOrderIndicesEstimate()
>>> foDist = result.getFirstOrderIndicesDistribution()
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getResult
"Accessor to the result.

Returns
-------
result : :class:`~openturns.SobolSimulationResult`
    The simulation result."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::setIndexQuantileLevel
"Accessor to the quantile level.

Parameters
----------
alpha : float
    The quantile level."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getIndexQuantileLevel
"Accessor to the quantile level.

Returns
-------
alpha : float
    The quantile level."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::setIndexQuantileEpsilon
"Accessor to the quantile tolerance.

Parameters
----------
epsilon : float
    The quantile tolerance"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getIndexQuantileEpsilon
"Accessor to the quantile tolerance.

Returns
-------
epsilon : float
    The quantile tolerance"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::drawFirstOrderIndexConvergence
"Draw the first order Sobol index convergence at a given level.

Parameters
----------
marginalIndex : int
    Index of the random vector component to consider
level : float, optional
    The Sobol index convergence is drawn at this given confidence level
    *level*. By default *level* is 0.95.

Returns
-------
graph : :class:`~openturns.Graph`
    Sobol index convergence graph"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::drawTotalOrderIndexConvergence
"Draw the total order Sobol index convergence at a given level.

Parameters
----------
marginalIndex : int
    Index of the random vector component to consider
level : float, optional
    The Sobol index convergence is drawn at this given confidence level
    *level*. By default *level* is 0.95.

Returns
-------
graph : :class:`~openturns.Graph`
    Sobol index convergence graph"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::setEstimator
"Sobol estimator accessor.

Parameters
----------
estimator : :class:`~openturns.SobolIndicesAlgorithm`
    The estimator of the indices."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getEstimator
"Sobol estimator accessor.

Returns
-------
estimator : :class:`~openturns.SobolIndicesAlgorithm`
    The estimator of the indices."

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::setExperimentSize
"Accessor to the experiment size.

Parameters
----------
size : int
    Internal size :math:`\sampleSize` of the Sobol design of experiment.
    See :class:`~openturns.SobolIndicesExperiment`"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getExperimentSize
"Accessor to the experiment size.

Returns
-------
size : int
    Internal size :math:`\sampleSize` of the Sobol design of experiment.
    See :class:`~openturns.SobolIndicesExperiment`"

// ---------------------------------------------------------------------

%feature("docstring") OT::SobolSimulationAlgorithm::getDistribution
"Accessor to the input distribution.

Returns
-------
distribution : :class:`~openturns.Distribution`
    Distribution of the random variable."
