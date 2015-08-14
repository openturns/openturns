// SWIG file HMatrixFactory.i

%{
#include "HMatrixFactory.hxx"
%}

%include HMatrixFactory.hxx
namespace OT { %extend HMatrixFactory { HMatrixFactory(const HMatrixFactory & other) { return new OT::HMatrixFactory(other); } } }
