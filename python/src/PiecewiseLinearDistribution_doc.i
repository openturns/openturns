%feature("docstring") OT::PiecewiseLinearDistribution
R"RAW(PiecewiseLinearDistribution distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

Its probability density function is defined as a piecewise linear function
given by its abscissae :math:`\vect{x} = (x_0, \dots, x_{n-1})` and
ordinates :math:`\vect{y} = (y_0, \dots, y_{n-1})` such that:

- :math:`\vect{x}` is strictly increasing: :math:`x_0 < x_1 < \dots < x_{n-1}`,
- :math:`\vect{y}` is nonnegative: :math:`y_i \geq 0` for all :math:`i`,
- the integral of the PDF over :math:`[x_0, x_{n-1}]` equals 1.

For :math:`x \in [x_i, x_{i+1}]`, the PDF is:

.. math::

    f_X(x) = y_i + \frac{y_{i+1} - y_i}{x_{i+1} - x_i} (x - x_i)

and :math:`f_X(x) = 0` outside :math:`[x_0, x_{n-1}]`.

The ordinates :math:`\vect{y}` are automatically rescaled by the constructor
so that the total integral equals 1.

The segment integrals :math:`\int_{x_i}^{x_{i+1}} f_X(x) \, dx` are precomputed
and stored internally for efficient CDF and probability computations.

Parameters
----------
x : sequence of float, size :math:`n \geq 2`
    Abscissae, strictly increasing.

y : sequence of float, size :math:`n`
    Ordinates (PDF values at the abscissae), nonnegative.

    Using the default constructor, the values are :math:`\vect{x} = (-1, 1)`
    and :math:`\vect{y} = (0.5, 0.5)`.

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distribution = otexp.PiecewiseLinearDistribution([0.0, 1.0, 2.0, 3.0], [0.0, 1.0, 1.0, 0.0])

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::PiecewiseLinearDistribution::getX
"Accessor to the distribution's abscissae.

Returns
-------
x : :class:`~openturns.Point`
    Abscissae, strictly increasing."

// ---------------------------------------------------------------------

%feature("docstring") OT::PiecewiseLinearDistribution::setX
"Accessor to the distribution's abscissae.

Parameters
----------
x : sequence of float, size :math:`n \geq 2`
    Abscissae, strictly increasing."

// ---------------------------------------------------------------------

%feature("docstring") OT::PiecewiseLinearDistribution::getY
"Accessor to the distribution's ordinates.

Returns
-------
y : :class:`~openturns.Point`
    Ordinates (PDF values at the abscissae)."

// ---------------------------------------------------------------------

%feature("docstring") OT::PiecewiseLinearDistribution::setY
"Accessor to the distribution's ordinates.

Parameters
----------
y : sequence of float, size :math:`n`
    Ordinates (PDF values at the abscissae), nonnegative."

// ---------------------------------------------------------------------

%feature("docstring") OT::PiecewiseLinearDistribution::getSingularities
"Accessor to the singularities of the PDF, i.e. the points where the derivative is discontinuous.

Returns
-------
singularities : :class:`~openturns.Point`
    Interior knots where the piecewise linear PDF changes slope."
