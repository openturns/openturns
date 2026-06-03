%feature("docstring") OT::MultivariateUniform
R"RAW(MultivariateUniform distribution.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.



The MultivariateUniform distribution is defined by its lower bound vector
:math:`\vect{a}` and its upper bound vector :math:`\vect{b}`.
Its probability density function is defined as:

.. math::

    f_{\vect{X}}(\vect{x}) = \prod_{i=1}^d \frac{1}{b_i-a_i},\quad
    \vect{x} \in [a_1; b_1] \times \dots \times [a_d; b_d]

with :math:`a_i < b_i` for all :math:`i`.

Its first moments are defined as:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Expect{\vect{X}} & = & \frac{\vect{a}+\vect{b}}{2} \\
        \Cov{\vect{X}} & = & \diag{\frac{(b_i-a_i)^2}{12}}
    \end{eqnarray*}

Parameters
----------
a : sequence of float
    Lower bound vector.

    Default value is [-1.0].
b : sequence of float, :math:`b_i > a_i`
    Upper bound vector.

    Default value is [1.0].

Examples
--------
Create a distribution:

>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> distribution = otexp.MultivariateUniform([0.0, 1.0], [1.0, 3.0])

Draw a sample:

>>> sample = distribution.getSample(5)
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateUniform::getA
"Accessor to the distribution's lower bound vector.

Returns
-------
a : :class:`~openturns.Point`
    Lower bound vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateUniform::getB
"Accessor to the distribution's upper bound vector.

Returns
-------
b : :class:`~openturns.Point`
    Upper bound vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateUniform::setA
"Accessor to the distribution's lower bound vector.

Parameters
----------
a : sequence of float, :math:`a_i < b_i`
    Lower bound vector."

// ---------------------------------------------------------------------

%feature("docstring") OT::MultivariateUniform::setB
"Accessor to the distribution's upper bound vector.

Parameters
----------
b : sequence of float, :math:`a_i < b_i`
    Upper bound vector."
