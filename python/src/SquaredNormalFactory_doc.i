%feature("docstring") OT::SquaredNormalFactory
R"RAW(SquaredNormal factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Method of moments estimator:

Let :math:`\mu` and :math:`\sigma` be the parameters of the underlying
normal distribution :math:`\mathcal{N}(\mu, \sigma)`.

From the sample moments:

.. math::
    :nowrap:

    \begin{eqnarray*}
      m_1 &= \displaystyle \frac{1}{n} \sum_{i=1}^n x_i = \mu^2 + \sigma^2 \\
      m_2 &= \displaystyle \frac{1}{n} \sum_{i=1}^n (x_i - \overline{x})^2 = 4 \mu^2 \sigma^2 + 2 \sigma^4
    \end{eqnarray*}

Solving for :math:`\mu` and :math:`\sigma`:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \widehat{\sigma}^2 &= m_1 - \sqrt{m_1^2 - \frac{m_2}{2}} \\
      \widehat{\sigma}   &= \sqrt{\widehat{\sigma}^2} \\
      \widehat{\mu}      &= \sqrt{m_1 - \widehat{\sigma}^2}
    \end{eqnarray*}

See also
--------
openturns.DistributionFactory, openturns.SquaredNormal

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distribution = ot.SquaredNormal(1.0, 2.0)
>>> sample = distribution.getSample(30)
>>> factory = otexp.SquaredNormalFactory()
>>> estimated = factory.build(sample)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::SquaredNormalFactory::buildAsSquaredNormal
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsSquaredNormal()

    buildAsSquaredNormal(*sample*)

    buildAsSquaredNormal(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.SquaredNormal`.

Returns
-------
dist : :class:`~openturns.SquaredNormal`
    The estimated distribution as a SquaredNormal.
)RAW"
