%feature("docstring") OT::MaximumLikelihoodFactory
R"RAW(Maximum likelihood factory.

Parameters
----------
distribution : :class:`~openturns.Distribution`
    The parametric distribution :math:`p_{\vect{\theta}}`.

Notes
-----
This class implements the generic maximum likelihood estimation
which is detailed in :ref:`maximum_likelihood`.

Let us denote :math:`(\vect{x}_1, \dots, \vect{x}_n)` the sample, :math:`p_{\vect{\theta}}`
the density of the parametric distribution we want to fit to the sample, with the
parameter vector :math:`\vect{\theta} \in \Theta \in \Rset^p` .


The likelihood of the sample according to :math:`p_{\vect{\theta}}` is:

.. math::

    L(\vect{x}_1, \dots, \vect{x}_n; \vect{\theta}) = \prod_{i=1}^n p_{\vect{\theta}}(\vect{x}_i)
    
The log-likelihood is defined as:

.. math::

    \ell(\vect{x}_1, \dots, \vect{x}_n; \vect{\theta}) = \sum_{i=1}^n \log p_{\vect{\theta}}(\vect{x}_i)

The estimator of  :math:`\vect{\theta}` maximizes the log-likelihood:

.. math::

    \hat{\vect{\theta}} = \argmax_{\vect{\theta} \in \Theta} \log \ell (\vect{x}_1, \dots, \vect{x}_n; \vect{\theta}) 

See also
--------
DistributionFactory

Examples
--------

In the following example, we estimate the parameters of a `Normal` distribution 
with maximum likelihood estimation.

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> distribution = ot.Normal(0.9, 1.7)
>>> sample = distribution.getSample(10)
>>> factory = ot.MaximumLikelihoodFactory(ot.Normal())
>>> inf_distribution = factory.build(sample)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::setOptimizationAlgorithm
"Accessor to the optimization solver.

Parameters
----------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for the optimization of the log-likelihood."

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::getOptimizationAlgorithm
"Accessor to the optimization solver.

Returns
-------
solver : :class:`~openturns.OptimizationAlgorithm`
    The solver used for the optimization of the log-likelihood."

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::setOptimizationBounds
"Accessor to the optimization bounds.

Parameters
----------
bounds : :class:`~openturns.Interval`
    The bounds used for the optimization of the log-likelihood."

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::getOptimizationBounds
"Accessor to the optimization bounds.

Returns
-------
bounds : :class:`~openturns.Interval`
    The bounds used for the optimization of the log-likelihood."

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::setOptimizationInequalityConstraint
"Accessor to the optimization inequality constraint.

Parameters
----------
inequalityConstraint : :class:`~openturns.Function`
    The inequality constraint used for the optimization of the log-likelihood."

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::BuildGaussianEstimator
R"RAW(Compute the asymptotic distribution of the parameters.


Parameters
----------
distribution : :class:`~openturns.Distribution`
    Parametric distribution.
sample : 2-d sequence of float
    Data to infer.

Returns
-------
distribution : :class:`~openturns.Normal`
    Asymptotic normal distribution of :math:`\hat{\vect{\theta}}`.
    
Notes
-----
We assume that the parametric model is regular: then, the asymptotic distribution of :math:`\hat{\vect{\theta}}` is normal.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MaximumLikelihoodFactory::BuildEstimator
"Estimate the parameters  and the asymptotic distribution.


Parameters
----------
factory : :class:`~openturns.DistributionFactory`
    Distribution factory to infer the data
    
    
sample : 2-d sequence of float
    Data to infer
is_regular : bool
    Indicates whether the parametric distribution is regular.

Returns
-------
result : :class:`~openturns.DistributionFactoryResult`
    Result class providing the estimate and the asymptotic distribution.
    
Notes
-----
If the model is regular, the asymptotic distribution of the estimator is normal and we get it from the Delta method.

If the model is not regular, we use the Bootstrap method and the kernel smoothing method to get the asymptotic distribution of the estimator."
