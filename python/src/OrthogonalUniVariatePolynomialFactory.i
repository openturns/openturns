// SWIG file OrthogonalUniVariatePolynomialFactory.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Point & weightsOut ($*ltype temp) %{ temp = OT::Point(); $1 = &temp; %}
%typemap(argout) OT::Point & weightsOut %{ $result = OT_AppendOutput($result, SWIG_NewPointerObj(new OT::Point(*$1), SWIGTYPE_p_OT__Point,  SWIG_POINTER_OWN)); %}

%{
#include "openturns/OrthogonalUniVariatePolynomialFactory.hxx"
%}

%include OrthogonalUniVariatePolynomialFactory_doc.i

%copyctor OT::OrthogonalUniVariatePolynomialFactory;

%include openturns/OrthogonalUniVariatePolynomialFactory.hxx
