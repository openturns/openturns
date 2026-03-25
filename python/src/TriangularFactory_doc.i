%feature("docstring") OT::TriangularFactory
R"RAW(Triangular factory.

Notes
-----
The parameters are estimated by method of moments:

.. math::
    :nowrap:

    \begin{align*}
      \Hat{a}_n & = \left(1 - \frac{\mathrm{sign}\left(x_{(1,n)}\right)}{2 + n}\right) x_{(1,n)}\\
      \Hat{b}_n & = \left(1 + \frac{\mathrm{sign}\left(x_{(n,n)}\right)}{2 + n}\right) x_{(n,n)}\\
      \Hat{m}_n & = 3 \overline{x}_n - \Hat{a}_n - \Hat{b}_n
    \end{align*}

See also
--------
DistributionFactory, Triangular)RAW"

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

