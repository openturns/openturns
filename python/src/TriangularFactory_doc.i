%feature("docstring") OT::TriangularFactory
R"RAW(Triangular factory.

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

The mode is estimated by the method of moments:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \hat{m} = 3 \bar{x}_n - \hat{a} - \hat{b},
    \end{eqnarray*}

where :math:`\bar{x}_n` is the sample mean.

See also
--------
DistributionFactory, Triangular
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TriangularFactory::buildAsTriangular
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsTriangular()

    buildAsTriangular(*sample*)

    buildAsTriangular(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.Triangular`.

Returns
-------
dist : :class:`~openturns.Triangular`
    The estimated distribution as a Triangular.
    
    In the first usage, the default Triangular distribution is built."

