// SWIG file FiniteOrthonormalFunctionFactory.i

%{
#include "openturns/FiniteOrthonormalFunctionFactory.hxx"
%}

%include FiniteOrthonormalFunctionFactory_doc.i

%copyctor OT::FiniteOrthonormalFunctionFactory;

%typemap("out") const SquareMatrix & { $result = SWIG_NewPointerObj(new SquareMatrix(*$1), SWIGTYPE_p_OT__SquareMatrix, SWIG_POINTER_OWN); }

%include openturns/FiniteOrthonormalFunctionFactory.hxx
