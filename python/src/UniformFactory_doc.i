%feature("docstring") OT::UniformFactory
R"RAW(Uniform factory.

Notes
-----
Let :math:`n` be the sample size.
Let :math:`x_{(1)}` be the sample minimum and :math:`x_{(n)}` be the sample maximum.
Let :math:`\delta` be the sample range:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \delta = x_{(n)} - x_{(1)}.
    \end{eqnarray*}

Then the bounds are estimated by extending the sample range:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \hat{a} &=& x_{(1)} - \frac{\delta}{n + 2}, \\
      \hat{b} &=& x_{(n)} + \frac{\delta}{n + 2}.
    \end{eqnarray*}

The maximum likelihood estimators would be :math:`x_{(1)}` and :math:`x_{(n)}`.
The above estimators expand the sample range to account for the finite sample size.

See also
--------
DistributionFactory, Uniform
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::UniformFactory::buildAsUniform
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsUniform()

    buildAsUniform(*sample*)

    buildAsUniform(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.Uniform`.

Returns
-------
dist : :class:`~openturns.Uniform`
    The estimated distribution as a :class:`~openturns.Uniform`.
    
    In the first usage, the default :class:`~openturns.Uniform` distribution is built."

