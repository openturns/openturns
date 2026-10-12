%feature("docstring") OT::NormalCopulaFactory
R"RAW(Normal Copula factory.

Notes
-----
We note :math:`\Hat{\tau}_n` the Kendall-\ :math:`\tau` of the sample.

The correlation matrix :math:`\mat{R}_n` is first estimated using Kendall's tau by:

.. math::

    R_{n,ij} = \sin\left(\frac{\pi}{2}\Hat{\tau}_{n,ij}\right)_{\strut}

and if it fails, using Spearman's correlation:

.. math::

    R_{n,ij} = 2\sin\left(\frac{\pi}{6}\Hat{\rho}_{n,ij}\right)_{\strut}

If both estimates fail, the sample linear correlation is regularized
as :math:`(R_n + \epsilon I)/(1 + \epsilon)` with doubling
:math:`\epsilon` until positive definiteness, as a last resort.

The following :class:`~openturns.ResourceMap` key is used:

- ``NormalCopulaFactory-RegularizationEpsilon`` (``Scalar``, default: ``1.0e-12``): initial diagonal weight of the last-resort regularization, doubled until positive definiteness with 1.0 as upper bound.

    
See also
--------
DistributionFactory, NormalCopula)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::NormalCopulaFactory::buildAsNormalCopula
"Estimate the copula as native copula.

**Available usages**:

    buildAsNormalCopula()

    buildAsNormalCopula(*sample*)

    buildAsNormalCopula(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
   The parameters of the :class:`~openturns.NormalCopula`.

Returns
-------
copula : :class:`~openturns.NormalCopula`
    The estimated copula as a Normal copula.
    
    In the first usage, the default Normal copula is built."
