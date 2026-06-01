%feature("docstring") OT::VonMisesFisherFactory
R"RAW(VonMisesFisher factory.

Notes
-----
The estimation is based on the method of moments.

Given a sample :math:`\mathbf{X}_1,\dots,\mathbf{X}_n` of points on the unit sphere
:math:`\mathbb{S}_{p-1}` of :math:`\Rset^p`, the estimators are:

.. math::
    :nowrap:

    \begin{eqnarray*}
      \displaystyle\Hat{\boldsymbol{\mu}}_n &=& \frac{\bar{\mathbf{X}}}{\|\bar{\mathbf{X}}\|}\\[6pt]
      \displaystyle\Hat{\kappa}_n &\mbox{s.t.}& \frac{I_{\nu}(\Hat{\kappa}_n)}{I_{\nu-1}(\Hat{\kappa}_n)} = \|\bar{\mathbf{X}}\|
    \end{eqnarray*}

where :math:`\bar{\mathbf{X}}=\frac{1}{n}\sum_{i=1}^n \mathbf{X}_i` is the empirical mean,
:math:`\nu = p/2`, and :math:`I_\nu` is the modified Bessel function of the first kind of
order :math:`\nu`.

The nonlinear equation for :math:`\Hat{\kappa}_n` is solved with a Brent algorithm.

If :math:`\|\bar{\mathbf{X}}\| = 0`, the sample is assumed to be uniformly distributed
over the sphere and the default distribution with :math:`\kappa = 0` is returned.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Notes
-----
For the factory, the nonlinear equation for :math:`\Hat{\kappa}_n`
is solved with a Brent algorithm whose tolerances can be tuned through
the following keys in :class:`~openturns.ResourceMap`:

  - ``VonMisesFisherFactory-AbsolutePrecision``,
  - ``VonMisesFisherFactory-RelativePrecision``,
  - ``VonMisesFisherFactory-ResidualPrecision``,
  - ``VonMisesFisherFactory-MaximumIteration``.


See also
--------
DistributionFactory,
experimental.VonMisesFisher)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::VonMisesFisherFactory::buildAsVonMisesFisher
"Estimate the distribution as native distribution.

**Available usages**:

    buildAsVonMisesFisher()

    buildAsVonMisesFisher(*sample*)

    buildAsVonMisesFisher(*param*)

Parameters
----------
sample : 2-d sequence of float
    Data.
param : sequence of float
    The parameters of the :class:`~openturns.experimental.VonMisesFisher`.

Returns
-------
dist : :class:`~openturns.experimental.VonMisesFisher`
    The estimated distribution as a VonMisesFisher.

    In the first usage, the default VonMisesFisher distribution is built."
