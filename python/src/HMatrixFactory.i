// SWIG file HMatrixFactory.i

%{
#include "openturns/HMatrixFactory.hxx"
%}

%include openturns/HMatrixFactory.hxx
namespace OT { %extend HMatrixFactory { HMatrixFactory(const HMatrixFactory & other) { return new OT::HMatrixFactory(other); } } }
