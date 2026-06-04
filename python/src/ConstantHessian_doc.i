%feature("docstring") OT::ConstantHessian
"Constant hessian.

Parameters
----------
constant : 3-d sequence of float
    Constant value

Examples
--------
>>> import openturns as ot
>>> hessian = ot.ConstantHessian(ot.SymmetricTensor(2, 1))"

// ---------------------------------------------------------------------

%feature("docstring") OT::ConstantHessian::getConstant
"Constant accessor.

Returns
-------
constant : :class:`~openturns.SymmetricTensor`
    Constant value
"
