// SWIG file SpecFunc.i

%{
#include "openturns/SpecFunc.hxx"
%}

// do not pass arguments by reference, return them as list items
%typemap(in, numinputs=0) OT::Scalar & logValue ($*ltype temp) %{ temp = 0.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & logValue %{ $result = OT::AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%typemap(in, numinputs=0) OT::Scalar & cancellation ($*ltype temp) %{ temp = 0.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & cancellation %{ $result = OT::AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%typemap(in, numinputs=0) OT::Scalar & sign ($*ltype temp) %{ temp = 0.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & sign %{ $result = OT::AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%include SpecFunc_doc.i

%ignore OT::SpecFunc::BinomialCoefficient; // math.comb

%include openturns/SpecFunc.hxx
