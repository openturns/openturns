// SWIG file HMatrixParameters.i

%{
#include "openturns/HMatrixParameters.hxx"
%}

%include HMatrixParameters_doc.i

%ignore OT::HMatrixParameters::getCompressionMethodAsUnsignedInteger;

%include openturns/HMatrixParameters.hxx
namespace OT{ %extend HMatrixParameters { HMatrixParameters(const HMatrixParameters & other) { return new OT::HMatrixParameters(other); } } }
