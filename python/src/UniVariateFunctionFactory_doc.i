%define OT_UniVariateFunctionFactory_doc
"Base class for univariate function factories."
%enddef
%feature("docstring") OT::UniVariateFunctionFactory
OT_UniVariateFunctionFactory_doc

// ---------------------------------------------------------------------

%define OT_UniVariateFunctionFactory_build_doc
R"RAW(Build the :math:`n`-th order univariate function.

Parameters
----------
n : int, :math:`0 \leq n`
    Function order.

Returns
-------
function : :class:`~openturns.UniVariateFunction`
    Requested univariate function.)RAW"
%enddef
%feature("docstring") OT::UniVariateFunctionFactory::build
OT_UniVariateFunctionFactory_build_doc

