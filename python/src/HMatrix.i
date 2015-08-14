// SWIG file HMatrix.i

%{
#include "HMatrix.hxx"
%}

%template(HMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::HMatrixImplementation>;

%include HMatrix.hxx

namespace OT {

%extend HMatrix {

  HMatrix(const HMatrix & other) { return new OT::HMatrix(other); }

} // HMatrix
} // OT
