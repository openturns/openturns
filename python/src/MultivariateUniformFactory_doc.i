%feature("docstring") OT::MultivariateUniformFactory
R"RAW(MultivariateUniform factory.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The parameters are estimated by the method of moments:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \displaystyle\Hat{a}_{i,n} & = & x_{i,(1,n)}-\frac{x_{i,(n,n)}-x_{i,(1,n)}}{n+2}\\
      \displaystyle\Hat{b}_{i,n} & = & x_{i,(n,n)}+\frac{x_{i,(n,n)}-x_{i,(1,n)}}{n+2}
    \end{eqnarray*}

where :math:`x_{i,(1,n)}` and :math:`x_{i,(n,n)}` are the minimum and maximum
of the i-th marginal, and :math:`n` the sample size.

See also
--------
openturns.DistributionFactory, openturns.experimental.MultivariateUniform

Examples
--------
Create a distribution from a sample:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> sample = ot.Sample([[0.5, 1.3], [1.5, 1.7], [2.5, 1.9]])
>>> distribution = otexp.MultivariateUniformFactory().build(sample)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateUniformFactory::buildAsMultivariateUniform
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsMultivariateUniform()

    buildAsMultivariateUniform(*sample*)

    buildAsMultivariateUniform(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.MultivariateUniform`.

Returns
-------
dist : :class:`~openturns.MultivariateUniform`
    The estimated distribution as a :class:`openturns.MultivariateUniform`.
    
    In the first usage, the default :class:`openturns.MultivariateUniform` distribution is built."
