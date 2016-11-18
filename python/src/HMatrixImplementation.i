// SWIG file HMatrixImplementation.i

%{
#include "openturns/HMatrixImplementation.hxx"
%}

%include HMatrixImplementation_doc.i

%include openturns/HMatrixImplementation.hxx
namespace OT { %extend HMatrixImplementation { HMatrixImplementation(const HMatrixImplementation & other) { return new OT::HMatrixImplementation(other); } } }
