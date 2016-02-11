// SWIG file HMatrixImplementation.i

%{
#include "openturns/HMatrixImplementation.hxx"
%}

%include openturns/HMatrixImplementation.hxx
namespace OT { %extend HMatrixImplementation { HMatrixImplementation(const HMatrixImplementation & other) { return new OT::HMatrixImplementation(other); } } }
