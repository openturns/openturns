%feature("docstring") OT::TranslationFunction
R"RAW(Translation function.

Parameters
----------
constant : sequence of float
    Vector :math:`\vect{c}`.

See also
--------
Function

Notes
-----
This implements a :class:`~openturns.Function` :math:`f`
defined as :

.. math::

    f : & \Rset^n \rightarrow \Rset^n  \\
        & \vect{X} \mapsto \vect{X} + \vect{c}

where:

- :math:`\vect{c}` is the :math:`\Rset^n` vector defined by the object *constant*.)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TranslationFunction::getConstant
R"RAW(Accessor to the constant.

Returns
-------
constant : :class:`~openturns.Point`
    Vector :math:`\vect{c}`
)RAW"

// ---------------------------------------------------------------------

%feature("docstring") OT::TranslationFunction::setConstant
R"RAW(Accessor to the constant.

Parameters
----------
constant : sequence of float
    Vector :math:`\vect{c}`
)RAW"
