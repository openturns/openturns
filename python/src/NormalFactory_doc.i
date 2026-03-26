%feature("docstring") OT::NormalFactory
R"RAW(Normal factory.

Parameters
----------
robust : bool, optional
    Flag to select robust estimators of the parameters.
    
    By default, robust is False.

Notes
-----    
The parameters are estimated by likelihood maximization if `robust=False`:

.. math::
    :nowrap:
    \begin{align*}
    \widehat{\vect{\mu}} & = \overline{\vect{x}}_{\sampleSize}\\
    \widehat{C} & = \frac{1}{\sampleSize - 1} \sum_{i=1}^{\sampleSize} \left(\vect{x}_i - \widehat{\vect{\mu}} \right)
      \left(\vect{x}_i - \widehat{\vect{\mu}}\right)^\top
    \end{align*}

where :math:`\overline{\vect{x}}_{\sampleSize}` is the sample mean:

.. math::
    \overline{\vect{x}}_{\sampleSize} = \frac{1}{\sampleSize} \sum_{i=1}^{\sampleSize} \vect{x}_i.

If `robust=True`, the estimation is performed using a robust estimator.
The estimate of :math:`\mu` is the empirical median :math:`q_{\sampleSize, 0.5}`.
The standard deviation is estimated based on the empirical inter-quartile:

.. math::
    \widehat{\sigma} = \frac{q_{\sampleSize, 0.75} - q_{\sampleSize, 0.25}}{a_{0.75} - a_{0.25}}

where :math:`a_{0.75}` and :math:`a_{0.25}` are the 75% and 25% quantiles
of the standard normal distribution and :math:`q_{\sampleSize, 0.75}`
and :math:`q_{\sampleSize, 0.25}` are the sample quantiles of levels 75% and 25%.
The correlation matrix :math:`R_\sampleSiz` is
estimated as the shape matrix of the underlying
:class:`~openturns.NormalCopula` using :class:`~openturns.NormalCopulaFactory`.

See also
--------
DistributionFactory, Normal, NormalCopulaFactory

Examples
--------

In the following example, the parameters of a :class:`~openturns.Normal` 
are estimated from a sample. 

>>> import openturns as ot
>>> ot.RandomGenerator.SetSeed(0)
>>> size = 10000
>>> distribution = ot.Normal(1.0, 2.0)
>>> sample = distribution.getSample(size)
>>> factory = ot.NormalFactory()
>>> estimated = factory.build(sample)
)RAW"

// ----------------------------------------------------------------------------

%feature("docstring") OT::NormalFactory::buildAsNormal
"Estimate the parameters of the distribution as a native distribution.

**Available usages**:

    buildAsNormal()

    buildAsNormal(*sample*)

    buildAsNormal(*param*)

Parameters
----------
sample : 2-d sequence of float
    Sample from which the distribution parameters are estimated.
param : sequence of float
    The parameters of the :class:`~openturns.Normal`.

Returns
-------
distribution : :class:`~openturns.Normal`
    The estimated distribution as a Normal.
    
    In the first usage, the default Normal distribution is built."
