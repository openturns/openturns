%feature("docstring") OT::MinMaxScaling
R"RAW(Min-max scaling.

.. warning::
    This class is experimental and likely to be modified in future releases.
    To use it, import the ``openturns.experimental`` submodule.

The min-max scaling maps each component :math:`x_i` to the unit interval:

.. math::

    u_i = \frac{x_i - \min_i}{\max_i - \min_i},
    \qquad \forall i \in \{1, \dots, d\}

with the inverse mapping :math:`x_i = \min_i + (\max_i - \min_i) u_i`.

Parameters
----------
minimum : sequence of float
    Lower bound of each component.
maximum : sequence of float
    Upper bound of each component, strictly greater than *minimum*.

Examples
--------
>>> import openturns as ot
>>> import openturns.experimental as otexp
>>> scaling = otexp.MinMaxScaling([1.0, 2.0], [3.0, 6.0])
>>> print(scaling.getTransform()([2.0, 4.0]))
[0.5,0.5]
>>> print(scaling.getInverseTransform()([0.5, 0.5]))
[2,4])RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::MinMaxScaling::getMinimum
"Accessor to the lower bound.

Returns
-------
minimum : :class:`~openturns.Point`
    Lower bound of each component."

// ---------------------------------------------------------------------

%feature("docstring") OT::MinMaxScaling::getMaximum
"Accessor to the upper bound.

Returns
-------
maximum : :class:`~openturns.Point`
    Upper bound of each component."

// ---------------------------------------------------------------------

%feature("docstring") OT::MinMaxScaling::getTransform
"Accessor to the forward scaling.

Returns
-------
transform : :class:`~openturns.Function`
    The :math:`x \mapsto u` mapping."

// ---------------------------------------------------------------------

%feature("docstring") OT::MinMaxScaling::getInverseTransform
"Accessor to the inverse scaling.

Returns
-------
transform : :class:`~openturns.Function`
    The :math:`u \mapsto x` mapping."
