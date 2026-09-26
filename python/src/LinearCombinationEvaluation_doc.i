%define OT_LinearCombinationEvaluation_doc
R"RAW(Base class.

Notes
-----
The following :class:`~openturns.ResourceMap` key is used:

- ``LinearCombinationEvaluation-SmallCoefficient`` (``Scalar``, default: ``0``)
)RAW"
%enddef
%feature("docstring") OT::LinearCombinationEvaluation::getCoefficients
"Accessor to the coefficients.

Returns
-------
coefficients : :class:`~openturns.Point`
    The coefficients."



%feature("docstring") OT::LinearCombinationEvaluation::getFunctionsCollection
"Accessor to the collection of functions.

Returns
-------
functions : list of :class:`~openturns.Function`
    The collection of functions."



%feature("docstring") OT::LinearCombinationEvaluation::setFunctionsCollectionAndCoefficients
"Set the collection of functions and coefficients.

Parameters
----------
functionsCollection : list of :class:`~openturns.Function`
    The collection of functions.
coefficients : :class:`~openturns.Point`
    The coefficients."
