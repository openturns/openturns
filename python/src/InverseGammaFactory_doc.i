%feature("docstring") OT::InverseGammaFactory
R"RAW(InverseGamma factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Method of moments estimator:

Let :math:`k` and :math:`\lambda` be the parameters of the inverse Gamma
distribution.

From the sample moments:

.. math::
    :nowrap:

    \begin{eqnarray*}
      m_1 &= \displaystyle \frac{1}{n} \sum_{i=1}^n x_i = \frac{1}{\lambda (k - 1)} \\
      m_2 &= \displaystyle \frac{1}{n} \sum_{i=1}^n (x_i - \overline{x})^2 = \frac{1}{\lambda^2 (k - 1)^2 (k - 2)}
    \end{eqnarray*}

Solving for :math:`k` and :math:`\lambda`:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \widehat{k}      &= \frac{m_1^2}{m_2} + 2 \\
      \widehat{\lambda} &= \frac{1}{m_1 (\widehat{k} - 1)}
    \end{eqnarray*}

See also
--------
openturns.DistributionFactory, openturns.InverseGamma

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distribution = ot.InverseGamma(5.0, 3.0)
>>> sample = distribution.getSample(30)
>>> factory = otexp.InverseGammaFactory()
>>> estimated = factory.build(sample)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::InverseGammaFactory::buildAsInverseGamma
R"RAW(Estimate the distribution as native distribution.

**Available usages**:

    buildAsInverseGamma()

    buildAsInverseGamma(*sample*)

    buildAsInverseGamma(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data. Must be a 1-d sample (dimension 1) of strictly positive values.
param : sequence of float
    The parameters of the :class:`~openturns.InverseGamma`.

Returns
-------
dist : :class:`~openturns.InverseGamma`
    The estimated distribution as an InverseGamma.
)RAW"
