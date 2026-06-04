%feature("docstring") OT::ConstantGradient
"Constant gradient.

Parameters
----------
constant : 2-d sequence of float
    Constant value

Examples
--------
>>> import openturns as ot
>>> gradient = ot.ConstantGradient(ot.Matrix([[1.0, 2.0, 3.0]]))"

// ---------------------------------------------------------------------

%feature("docstring") OT::ConstantGradient::getConstant
"Constant accessor.

Returns
-------
constant : :class:`~openturns.Matrix`
    Constant value
"
