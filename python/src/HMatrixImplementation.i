// SWIG file HMatrixImplementation.i

%{
#include "HMatrixImplementation.hxx"
%}

%include HMatrixImplementation.hxx
namespace OT { %extend HMatrixImplementation { HMatrixImplementation(const HMatrixImplementation & other) { return new OT::HMatrixImplementation(other); } } }
