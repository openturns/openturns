// SWIG file FittingTest.i


// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::TestResult & bestResultOut ($*ltype temp) %{ temp = OT::TestResult(); $1 = &temp; %}
%typemap(argout) OT::TestResult & bestResultOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::TestResult(*$1), SWIGTYPE_p_OT__TestResult, SWIG_POINTER_OWN |  0 )); %}

%typemap(in, numinputs=0) OT::Scalar & bestBICOut ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & bestBICOut %{ $result = SWIG_Python_AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%typemap(in, numinputs=0) OT::Distribution & estimatedDistribution ($*ltype temp) %{ temp = OT::Distribution(); $1 = &temp; %}
%typemap(argout) OT::Distribution & estimatedDistribution %{ $result = SWIG_Python_AppendOutput( SWIG_NewPointerObj(new OT::Distribution(*$1), SWIGTYPE_p_OT__Distribution, SWIG_POINTER_OWN |  0 ), $result)  ; %}

%typemap(in, numinputs=0) OT::Scalar & bestAICOut ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & bestAICOut %{ $result = SWIG_Python_AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%{
#include "openturns/FittingTest.hxx"
%}

%include FittingTest_doc.i

%include openturns/FittingTest.hxx
