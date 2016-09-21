// SWIG file HMatrix.i

%{
#include "openturns/HMatrix.hxx"
%}

%template(HMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::HMatrixImplementation>;
%template(pairlonglong) std::pair< size_t, size_t >;

%include openturns/HMatrix.hxx

namespace OT {

%extend HMatrix {

  HMatrix(const HMatrix & other) { return new OT::HMatrix(other); }

} // HMatrix
} // OT
