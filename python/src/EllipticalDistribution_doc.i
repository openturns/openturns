%feature("docstring") OT::EllipticalDistribution
R"RAW(Base class for elliptical distributions.

Notes
-----
The class :class:`~openturns.EllipticalDistribution` maps the concept of elliptical distributions.

The distribution of the random vector :math:`\vect{X}` is elliptical if its characteristic function is defined by:

.. math::
    :label: shapeMatrix1

    \phi_{\vect{X} - \vect{\mu}}(\vect{t}) = g(\Tr{\vect{t}}\mat{\Sigma}\vect{t})

where :math:`\vect{\mu}` is a location vector, :math:`\mat{\Sigma}` a nonnegative-definite matrix called the *shape
matrix* and :math:`g` a scalar function.

The probability density function is defined as:

.. math::
  :label: generator

  f_{\vect{X}}(\vect{x}) = \varphi \left( \Tr{(\vect{x} - \vect{\mu})} \mat{\Sigma}^{-1} (\vect{x} - \vect{\mu})\right)

The fonction :math:`\varphi` is called the *density generator*.

We can define the matrix :math:`\mat{R}` as: 

.. math::
  :label: shapeMatrix2

  \mat{\Sigma} = \diag (\vect{\sigma}) \mat{R} \diag (\vect{\sigma})
  
When the distribution has a mean, then it is :math:`\vect{\mu}`.

When the distribution has a covariance matrix, then it is proportional to  :math:`\mat{\Sigma}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::computeLogDensityGenerator
R"RAW(Computes the log of the density generator.

Parameters
----------
y : float
    Input.

Returns
-------
p : float
    Value :math:`\log \varphi(y)`.

Notes
-----
The density generator  :math:`\varphi` is defined in :eq:`generator`.

See Also
--------
isElliptical, computePDF)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::computeDensityGenerator
R"RAW(Compute the density generator.

Parameters
----------
y : float
    Input.

Returns
-------
p : float
    Value :math:`\varphi(y)`.

Notes
-----
The density generator is the function :math:`\varphi` such that the probability density function
of the elliptical distribution rewrites:

.. math::

    f_{\vect{X}}(\vect{x}) =
        \varphi\left(\Tr{\left(\vect{x} - \vect{\mu}\right)}
                      \mat{\Sigma}^{-1}
                      \left(\vect{x} - \vect{\mu}\right)
            \right),
        \quad \vect{x} \in \supp{\vect{X}}

The density generator only exists for elliptical distributions.

See Also
--------
isElliptical, computePDF)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::computeDensityGeneratorDerivative
R"RAW(Compute the first-order derivative of the probability density function.

PDF of the characteristic generator of the elliptical distribution.

Parameters
----------
beta2 : float
    Density generator input.

Returns
-------
p : float
    Value :math:`\dfrac{d\varphi}{d\,t}(y)`.

Notes
-----
This function only exists for elliptical distributions.

See Also
--------
isElliptical, computeDensityGenerator)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::computeDensityGeneratorSecondDerivative
R"RAW(Compute the second-order derivative of the probability density function.

PDF of the characteristic generator of the elliptical distribution.

Parameters
----------
beta2 : float
    Density generator input.

Returns
-------
p : float
    Value :math:`\dfrac{d^2\varphi}{d\,t^2}(y)`.

Notes
-----
This function only exists for elliptical distributions.

See Also
--------
isElliptical, computeDensityGenerator)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::getSigma
R"RAW(Accessor to the scale parameter :math:`\vect{\sigma}`.

Returns
-------
sigma : float or sequence of positive float
    The scale parameter :math:`\vect{\sigma}`.

Notes
-----
The scale parameter :math:`\vect{\sigma}` is defined in :eq:`shapeMatrix2`.
)RAW"
// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::setSigma
R"RAW(Accessor to the scale parameter :math:`\vect{\sigma}`.

Parameters
----------
sigma : float or sequence of positive float
    The scale parameter :math:`\vect{\sigma}`.

Notes
-----
The scale parameter :math:`\vect{\sigma}` is defined in :eq:`shapeMatrix2`.)RAW"


// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::setR
R"RAW(Accessor to the :math:`\mat{R}` matrix.

Parameters
----------
R : sequence of positive float
    The matrix :math:`\mat{R}`.

Notes
-----
The :math:`\mat{R}` matrix is defined in :eq:`shapeMatrix2`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::getR
R"RAW(Accessor to the :math:`\mat{R}` matrix.

Returns
-------
R : :class:`openturns.CorrelationMatrix`
    The matrix :math:`\mat{R}`.

Notes
-----
The :math:`\mat{R}` matrix is defined in :eq:`shapeMatrix2`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::setMu
R"RAW(Accessor to the location parameter :math:`\vect{\mu}`.

Parameters
----------
mu : sequence of float
    The location parameter :math:`\vect{\mu}`.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::EllipticalDistribution::getMu
R"RAW(Accessor to the location parameter :math:`\vect{\mu}`.

Parameters
----------
mu : sequence of float
    The location parameter :math:`\vect{\mu}`.)RAW"
