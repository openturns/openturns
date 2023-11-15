// SWIG file HMatrixParameters.i

%{
#include "openturns/HMatrixParameters.hxx"
%}

%include HMatrixParameters_doc.i

%ignore OT::HMatrixParameters::getCompressionMethodAsUnsignedInteger;

%copyctor OT::HMatrixParameters;

%include openturns/HMatrixParameters.hxx
