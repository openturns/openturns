%feature("docstring") OT::MuSigmaScaling
R"RAW(Standardization scaling.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The standardization (or :math:`\mu-\sigma` scaling) maps each component
:math:`x_i` to:

.. math::

    z_i = \frac{x_i - \mu_i}{\sigma_i},
    \qquad \forall i \in \{1, \dots, d\}

with the inverse mapping :math:`x_i = \mu_i + \sigma_i z_i`.
It centers the data and normalizes the dispersion of each marginal,
which improves the conditioning of regression designs and the accuracy
of hyperparameter optimization.

Parameters
----------
mu : sequence of float
    Location of each component.
sigma : sequence of float
    Positive scale of each component.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> scaling = otexp.MuSigmaScaling([1.0, 2.0], [2.0, 4.0])
>>> print(scaling.getTransform()([3.0, 6.0]))
[1,1]
>>> print(scaling.getInverseTransform()([1.0, 1.0]))
[3,6])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MuSigmaScaling::getMu
"Accessor to the location.

Returns
-------
mu : :class:`~openturns.Point`
    Location of each component."

// ---------------------------------------------------------------------

%feature("docstring") OT::MuSigmaScaling::getSigma
"Accessor to the scale.

Returns
-------
sigma : :class:`~openturns.Point`
    Positive scale of each component."

// ---------------------------------------------------------------------

%feature("docstring") OT::MuSigmaScaling::getTransform
"Accessor to the forward scaling.

Returns
-------
transform : :class:`~openturns.Function`
    The :math:`x \mapsto z` mapping."

// ---------------------------------------------------------------------

%feature("docstring") OT::MuSigmaScaling::getInverseTransform
"Accessor to the inverse scaling.

Returns
-------
transform : :class:`~openturns.Function`
    The :math:`z \mapsto x` mapping."
