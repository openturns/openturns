%feature("docstring") OT::Dirichlet
R"RAW(Dirichlet distribution.

The Dirichlet distribution [kotz2000]_ is used to model a random vector
:math:`\inputRV = (X_1, \dots, X_{\inputDim+1})` in the unit simplex:

.. math::

    & \sum_{i=1}^{\inputDim+1} X_i = 1 \\
    & X_i \in [0,1] \quad \forall 1 \leq i \leq \inputDim + 1.

This relation implies that :math:`X_{\inputDim +1} = 1 - \sum_{i=1}^{\inputDim} X_i`.
Then, the last component is fully determined by the other components. The
random vector is actually of dimension :math:`\inputDim`.

The Dirichlet distribution implemented in the library models the random
vector :math:`\inputRV = (X_1, \dots, X_{\inputDim})` of dimension
:math:`\inputDim` and such that:

.. math::

    & \sum_{i=1}^{\inputDim} X_i \leq 1 \\
    & X_i \in [0,1] \quad \forall 1 \leq i \leq \inputDim.

Let :math:`\vect{\theta} = (\theta_1, \dots, \theta_{\inputDim+1})` be a
vector with strictly positive components. Let
:math:`\norm{\vect{\theta}}_1 = \sum_{i=1}^{\inputDim+1} \theta_i` be the
1-norm of :math:`\vect{\theta}` and let the multivariate Beta function be
defined in terms of the Gamma function:

.. math::

    B(\vect{\theta}) = \frac{\prod_{i=1}^{\inputDim+1}\Gamma(\theta_i)}
                            {\Gamma(\norm{\vect{\theta}}_1)}.

The Dirichlet probability density function is defined as:

.. math::

    \pdf = \frac{1}{B(\vect{\theta})}
            \left( 1  - \sum_{i=1}^{\inputDim} x_i \right)^{\theta_{\inputDim+1}-1}
            \prod_{i=1}^{\inputDim} x_i^{\theta_i - 1},
    \quad \inputReal \in \supp{\inputRV}

where:

.. math::

    \supp{\inputRV} = \left\{ \inputReal \in [0,1]^{\inputDim} \mid
    \norm{\inputReal}_1 \leq 1 \right\}.

Its first moments are:

.. math::

    & \Expect{\inputRV} = \frac{1}{\norm{\vect{\theta}}_1} \vect{\theta}, \\
    & \Cov{\inputRV} = \frac{1}{\norm{\vect{\theta}}_1^2(\norm{\vect{\theta}}_1+1)}
      \left( \delta_{ij}\theta_i\norm{\vect{\theta}}_1
      - \theta_i\theta_j \right)_{1 \leq i,j \leq \inputDim},

where :math:`\delta_{ij} = 1` if :math:`i = j` and :math:`\delta_{ij} = 0`
otherwise.

The cumulative distribution function
:math:`F(\inputReal) = \Prob{\inputRV \leq \inputReal}` is evaluated
deterministically, following the recursive algorithms and bounds described
in [gouda2005]_. Inside the simplex, the CDF is evaluated from the
Lauricella hypergeometric function :math:`F_A` [lauricella1893]_, using the
Exton representation [exton1976]_:

.. math::

    F(\inputReal) = \frac{\Gamma(\norm{\vect{\theta}}_1)}
    {\Gamma(\theta_{\inputDim+1})\prod_{i=1}^{\inputDim}\Gamma(\theta_i+1)}
    \left(\prod_{i=1}^{\inputDim}x_i^{\theta_i}\right)
    F_A\left(1-\theta_{\inputDim+1};
    \theta_1,\dots,\theta_{\inputDim};
    \theta_1+1,\dots,\theta_{\inputDim}+1;
    x_1,\dots,x_{\inputDim}\right).

When :math:`\theta_{\inputDim+1}` is an integer, expanding
:math:`\left(1-\sum_{i=1}^{\inputDim} x_i\right)^{\theta_{\inputDim+1}-1}`
gives an exact finite series of incomplete beta functions. Outside the
simplex, the CDF follows from inclusion-exclusion over the upper corners
:math:`\{X_i > x_i, i \in S\}`; each term is integrated by Gauss-Jacobi
quadrature. A normal orthant approximation is used when all the shape
parameters and their sum are large. The upper-corner quadrature is used up to
a maximum dimension that depends on the shape parameters; beyond this limit,
the CDF is bracketed by the Frechet bounds of its beta marginals and their
midpoint is returned with a warning. The Lauricella series is evaluated with
the :class:`~openturns.ResourceMap` keys listed in the Notes section.

The density is evaluated on the closed simplex whenever its limit is finite.
A zero component :math:`x_i` contributes a zero factor if :math:`\theta_i>1`
and a unit factor if :math:`\theta_i=1`; the same convention applies to the
slack :math:`1-\sum_{i=1}^{\inputDim} x_i` with
:math:`\theta_{\inputDim+1}`. Singular exponents :math:`\theta_i<1` keep the
zero convention of the Beta distribution at the boundary.

Parameters
----------
theta : sequence of float, optional
    The :math:`(\theta_1, \dots, \theta_{\inputDim+1})` parameter
    with :math:`\theta_i > 0` for all :math:`i` and :math:`\inputDim \geq 1`.

    Default value is [1.0, 1.0].

See Also
--------
Multinomial

Notes
-----
The following :class:`~openturns.ResourceMap` keys are used:

- ``Dirichlet-CDFBoxTolerance`` (``Scalar``, default: ``1.0e-10``): relative tolerance of the adaptive box quadrature.
- ``Dirichlet-CDFExtonMaximumCancellation`` (``Scalar``, default: ``1.0e4``): maximum cancellation level accepted for the Exton series.
- ``Dirichlet-CDFMaximumBoxDimension`` (``UnsignedInteger``, default: ``7``): maximum dimension for which the box quadrature is attempted.
- ``Dirichlet-CDFMaximumIntegerBeta`` (``UnsignedInteger``, default: ``128``): maximum integer exponent for the finite incomplete-beta series.
- ``Dirichlet-CDFMaximumQuadratureDimension`` (``UnsignedInteger``, default: ``10``): maximum dimension for the upper-corner inclusion-exclusion and the recursive integration.
- ``Dirichlet-CDFMaximumTensorNodes`` (``UnsignedInteger``, default: ``20000000``): maximum number of tensor-product nodes in the box quadrature.
- ``Dirichlet-CDFNormalMinimumProbability`` (``Scalar``, default: ``1.0e-12``): minimum probability for which the normal approximation is accepted.
- ``Dirichlet-CDFNormalMinimumShape`` (``Scalar``, default: ``100.0``): minimum shape parameter for the normal approximation.
- ``Dirichlet-CDFNormalMinimumTotal`` (``Scalar``, default: ``1.0e5``): minimum total shape parameter for the normal approximation.
- ``Dirichlet-CDFUpperMaximumCancellation`` (``Scalar``, default: ``1.0e13``): maximum cancellation level accepted for the upper-corner inclusion-exclusion.
- ``Dirichlet-DefaultIntegrationSize`` (``UnsignedInteger``, default: ``50``): maximum order of the deterministic numerical integration used by the CDF.

Examples
--------
Create a distribution of dimension :math:`\inputDim = 2`:

>>> import openturns as ot
>>> distribution = ot.Dirichlet([1.0, 1.0, 1.0])

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Dirichlet::getTheta
R"RAW(Accessor to the distribution's vector parameter.

Returns
-------
theta : sequence of float, :class:`~openturns.Point`,
        The :math:`(\theta_1, \dots, \theta_{\inputDim+1})` parameter.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::Dirichlet::setTheta
R"RAW(Accessor to the distribution's vector parameter.

Parameters
----------
theta : sequence of float, 
        The :math:`(\theta_1, \dots, \theta_{\inputDim+1})` parameter
        with :math:`\theta_i > 0` for all :math:`i` and :math:`\inputDim \geq 1`)RAW"
