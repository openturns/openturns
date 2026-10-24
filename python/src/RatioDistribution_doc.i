%feature("docstring") OT::RatioDistribution
R"RAW(Ratio distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The distribution of the random variable :math:`Z = X / Y`, where :math:`X` and
:math:`Y` are two independent univariate continuous random variables with
probability density functions :math:`f_X` and :math:`f_Y`.

The probability density function of :math:`Z` is:

.. math::

    \displaystyle f_Z(z) = \int_{\Rset} \left|u\right| f_X(z u) f_Y(u) \Diff u.

Its cumulative distribution function is:

.. math::

    \displaystyle F_Z(z) = \int_{-\infty}^{0} f_Y(u) \left(1 - F_X(z u)\right) \Diff u
    + \int_{0}^{+\infty} f_Y(u) F_X(z u) \Diff u.

When the support of :math:`Y` does not contain zero, the raw moments of
:math:`Z` are finite as soon as those of :math:`X` and :math:`1 / Y` are, and:

.. math::

    \Expect{Z^n} = \Expect{X^n} \Expect{Y^{-n}}.

In particular:

.. math::

    \Expect{Z} = \Expect{X} \Expect{1 / Y}.

When the support of :math:`Y` contains zero in its interior, the ratio
distribution has heavy tails and its moments are not defined. When zero is only
an endpoint of the support of :math:`Y`, the inverse moments are finite when
the density of :math:`Y` vanishes fast enough at zero; they are integrated
numerically, and the computation raises an exception when the quadrature
detects a divergence.

Parameters
----------

distX : :class:`~openturns.Distribution`, univariate
    Distribution of the numerator. It must be continuous.

distY : :class:`~openturns.Distribution`, univariate
    Distribution of the denominator. It must be continuous and independent of
    `distX`.

Returns
-------
distZ : :class:`~openturns.Distribution`, univariate
    Distribution of the random variable :math:`Z = X / Y`.

See Also
--------
:class:`~openturns.ProductDistribution`, :class:`~openturns.Distribution`

Examples
--------
Create the ratio of two independent standard normal random variables:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distZ = otexp.RatioDistribution(ot.Normal(), ot.Normal())
>>> print(round(distZ.computePDF(0.0), 6))
0.31831

The division operator creates the same distribution:

>>> distZ = ot.Normal() / ot.Normal()
>>> print(distZ.getImplementation().getClassName())
RatioDistribution

Notes
-----
The probability density function and the cumulative distribution function are
evaluated by adaptive one-dimensional quadrature on the support of the
denominator. When this support contains zero in its interior, the distribution
has unbounded support and its moments are reported as not defined. When zero is
only an endpoint, the inverse moments are integrated with the same adaptive
rule and an exception is raised when the estimated error indicates a
divergence.
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::RatioDistribution::setLeft
"Accessor to the numerator distribution.

Parameters
----------
distX : :class:`~openturns.Distribution`, univariate
    Continuous distribution of the numerator."

// ---------------------------------------------------------------------

%feature("docstring") OT::RatioDistribution::setRight
"Accessor to the denominator distribution.

Parameters
----------
distY : :class:`~openturns.Distribution`, univariate
    Continuous distribution of the denominator."

// ---------------------------------------------------------------------

%feature("docstring") OT::RatioDistribution::getLeft
"Accessor to the numerator distribution.

Returns
-------
distX : :class:`~openturns.Distribution`, univariate
    Distribution of the numerator."

// ---------------------------------------------------------------------

%feature("docstring") OT::RatioDistribution::getRight
"Accessor to the denominator distribution.

Returns
-------
distY : :class:`~openturns.Distribution`, univariate
    Distribution of the denominator."
