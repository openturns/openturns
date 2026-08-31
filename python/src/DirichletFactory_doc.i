%feature("docstring") OT::DirichletFactory
"Dirichlet factory.

Notes
-----
The estimation is done by maximizing the likelihood of the sample,
using the algorithm described in [minka2012]_.

See also
--------
DistributionFactory, Dirichlet"

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
    The estimated distribution as a :class:`openturns.Dirichlet`.
    

Notes
-----
This class uses the following entries of :class:`~openturns.ResourceMap`:

- *DirichletFactory-MaximumIteration* (:raw:`UnsignedInteger`, default: `10`)
- *DirichletFactory-ParametersEpsilon* (:raw:`Scalar`, default: `1e-12`)

    In the first usage, the default :class:`openturns.Dirichlet` distribution is built."


