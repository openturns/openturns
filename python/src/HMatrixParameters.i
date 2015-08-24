// SWIG file HMatrixParameters.i

%{
#include "HMatrixParameters.hxx"
%}

%include HMatrixParameters_doc.i

%include HMatrixParameters.hxx
namespace OT{ %extend HMatrixParameters { HMatrixParameters(const HMatrixParameters & other) { return new OT::HMatrixParameters(other); } } }
