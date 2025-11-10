// SWIG file QuantileConfidenceInteval.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::Scalar & coverageOut ($*ltype temp) %{ temp = -1.0; $1 = &temp; %}
%typemap(argout) OT::Scalar & coverageOut %{ $result = OT::AppendOutput($result, OT::convert<OT::Scalar, OT::_PyFloat_>(*$1)); %}

%{
#include "openturns/QuantileConfidence.hxx"
%}

%include QuantileConfidence_doc.i

%copyctor OT::QuantileConfidence;

%include openturns/QuantileConfidence.hxx

