%feature("docstring") OT::GumbelCopulaFactory
R"RAW(Gumbel Copula factory.

Notes
-----
We note :math:`\Hat{\tau}_n` the Kendall-\ :math:`\tau` of the sample.

We use the following estimator:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \displaystyle \Hat{\theta}_n=\frac{1^{\strut}}{1 - \Hat{\tau}_{n_{\strut}}}
    \end{eqnarray*}

See also
--------
DistributionFactory, GumbelCopula)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::GumbelCopulaFactory::buildAsGumbelCopula
R"RAW(Estimate the copula as native copula.

**Available usages**:

    buildAsGumbelCopula()

    buildAsGumbelCopula(*sample*)

    buildAsGumbelCopula(*param*)

Parameters
----------
sample : 2-d sequence of float,
    Data of dimension 2.
param : sequence of float of size 1,
   The parameter :math:`\theta`.

Returns
-------
copula : :class:`~openturns.GumbelCopula`
    The estimated copula as a Gumbel copula.
    
    In the first usage, the default Gumbel copula is built.)RAW"
