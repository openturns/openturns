%feature("docstring") OT::VonMisesFisher
R"RAW(von Mises-Fisher distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The von Mises-Fisher distribution is defined on the unit sphere
:math:`\mathbb{S}_{p-1} = \{\mathbf{x}\in\mathbb{R}^p \mid \|\mathbf{x}\| = 1\}`
with probability density function:

.. math::

    f(\mathbf{x}) = C(\kappa)\,\exp\big(\kappa\,\mathbf{x}^T\boldsymbol{\mu}\big),
    \qquad \mathbf{x}\in\mathbb{S}_{p-1}

where :math:`\boldsymbol{\mu}\in\mathbb{S}_{p-1}` is the mean direction,
:math:`\kappa\ge 0` is the concentration parameter,
:math:`I_\nu` is the modified Bessel function of the first kind of order
:math:`\nu` and the normalization constant is:

.. math::

    C(\kappa) = \frac{\kappa^{p/2-1}}{(2\pi)^{p/2}\, I_{p/2-1}(\kappa)}.


When :math:`\kappa = 0`, the distribution is uniform over the unit sphere.

The **mean** is:

.. math::

    \mathbb{E}[\mathbf{X}] = \frac{I_{p/2}(\kappa)}{I_{p/2-1}(\kappa)}\,
    \boldsymbol{\mu}.

Note that the **mean** is not in the unit sphere, its norm is less than 1.
The **entropy** is:

.. math::

    \mathcal{H} = -\log C(\kappa) - \kappa\,\frac{I_{p/2}(\kappa)}{I_{p/2-1}(\kappa)}.

Sampling is performed using the fast rejection-sampling algorithm of
Pinzón and Jung [pinzon2023]_.

When :math:`p=2`, the von Mises-Fisher distribution is related to the
von Mises distribution the following way. The von Mises-Fisher distribution
is distributed over the unit circle while the von Mises distribution is the
corresponding univariate angular distribution.

As the von Mises-Fisher distribution has no density, most of its methods are
not implemented.

Parameters
----------
mu : 1-d sequence of float
    Direction, unit vector of dimension :math:`p \ge 2`.

    Default value is [1.0, 0.0].
kappa : float, :math:`\kappa \ge 0`
    Concentration (shape) parameter.

    Default value is 1.0.

See Also
--------
VonMisesFisherFactory
openturns.VonMises

Examples
--------
Create a distribution of dimension 3:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> ot.RandomGenerator.SetSeed(0)
>>> mu = [0.0, 0.0, 1.0]
>>> kappa = 5.0
>>> distribution = otexp.VonMisesFisher(mu, kappa)
>>> print(distribution)
VonMisesFisher(mu = [0,0,1], kappa = 5)

Get a realization:

>>> point = distribution.getRealization()
>>> print(point)
[0.0703119,-0.146377,0.986727]

Evaluate the log-PDF:

>>> pdf = distribution.computeLogPDF(point)
>>> print(pdf)
-0.294...
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::VonMisesFisher::getMu
R"RAW(Accessor to the mean direction parameter.

Returns
-------
mu : :class:`~openturns.Point`
    Unit-norm mean direction vector :math:`\boldsymbol{\mu}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::VonMisesFisher::setMu
R"RAW(Accessor to the mean direction parameter.

If the input vector is not of unit norm, it is normalized internally.

Parameters
----------
mu : 1-d sequence of float of size :math:`p \ge 2`
    Mean direction vector (will be normalized to unit length).)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::VonMisesFisher::getKappa
R"RAW(Accessor to the concentration parameter.

Returns
-------
kappa : float
    Concentration parameter :math:`\kappa`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::VonMisesFisher::setKappa
R"RAW(Accessor to the concentration parameter.

Parameters
----------
kappa : float, :math:`\kappa \ge 0`
    Concentration parameter.)RAW"
