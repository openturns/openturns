%feature("docstring") OT::DirichletFactory
R"RAW(Dirichlet factory.

The parameters of the :class:`~openturns.Dirichlet` distribution are
estimated by maximizing the likelihood of the sample, using the Newton and
fixed-point algorithms described in [minka2012]_.

Let :math:`\sampleSize` be the sample size and let
:math:`\left(\inputReal_i\right)_{1 \leq i \leq \sampleSize}` be the sample,
with :math:`\inputReal_i \in \Rset_{>0}^{\inputDim}` and
:math:`\sum_{j=1}^{\inputDim} x_{i,j} < 1` for all :math:`i`.
Let :math:`\vect{\theta} \in \Rset_{>0}^{\inputDim+1}` be the parameter of
the :class:`~openturns.Dirichlet` distribution and let
:math:`\norm{\vect{\theta}}_1 = \sum_{j=1}^{\inputDim+1} \theta_j`.
We note :math:`x_{i,\inputDim+1} = 1 - \sum_{j=1}^{\inputDim} x_{i,j}`.
The log-likelihood is:

.. math::

    \ell(\vect{\theta}) = \sampleSize \left( \log\Gamma(\norm{\vect{\theta}}_1)
    - \sum_{j=1}^{\inputDim+1} \log\Gamma(\theta_j) \right)
    + \sum_{j=1}^{\inputDim+1} (\theta_j - 1)
      \sum_{i=1}^{\sampleSize} \log x_{i,j}.

The maximum likelihood estimate
:math:`\hat{\vect{\theta}} = \argmax_{\vect{\theta}} \ell(\vect{\theta})`
is computed with the Newton iteration and the fixed-point update:

.. math::

    \theta_j^{(k+1)} = \psi^{-1}\left( \psi(\norm{\vect{\theta}^{(k)}}_1)
    + \frac{1}{\sampleSize} \sum_{i=1}^{\sampleSize} \log x_{i,j} \right),
    \quad j = 1, \dots, \inputDim+1,

where :math:`\psi` is the digamma function. The initial guess is obtained by
the method of moments. The algorithm stops when the norm of the parameter
update :math:`\norm{\vect{\theta}^{(k+1)} - \vect{\theta}^{(k)}}` is smaller
than :math:`\inputDim \times` ``DirichletFactory-ParametersEpsilon`` or after
``DirichletFactory-MaximumIteration`` iterations.

See also
--------
DistributionFactory, Dirichlet

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``DirichletFactory-MaximumIteration`` (``UnsignedInteger``, default: ``10``): maximum number of iterations
  of the likelihood maximization.
- ``DirichletFactory-ParametersEpsilon`` (``Scalar``, default: ``1e-12``): tolerance used as a stopping criterion
  of the likelihood maximization and as a lower bound for the estimated parameters.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::DirichletFactory::buildAsDirichlet
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsDirichlet()

    buildAsDirichlet(*sample*)

    buildAsDirichlet(*param*)

Parameters
----------
sample : 2-d sequence of float
    Sample from which the distribution parameters are estimated.
param : sequence of float
   The parameters of the :class:`~openturns.Dirichlet`.

Returns
-------
dist : :class:`~openturns.Dirichlet`
    The estimated distribution as a :class:`~openturns.Dirichlet`.
    

    In the first usage, the default :class:`~openturns.Dirichlet` distribution is built."


