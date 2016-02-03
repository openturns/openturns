// SWIG file HermitianMatrix.i

%{
#include "HermitianMatrix.hxx"
%}

%include HermitianMatrix_doc.i

%ignore OT::HermitianMatrix::checkHermitian;

%include HermitianMatrix.hxx

namespace OT {

%extend HermitianMatrix {

  HermitianMatrix(const HermitianMatrix & other) { return new OT::HermitianMatrix(other); }

  HermitianMatrix(PyObject * pyObj) { return new OT::HermitianMatrix( OT::convert<OT::_PySequence_,OT::HermitianMatrix>(pyObj) ); }

  OTComplexMatrixGetAccessors()

  HermitianMatrix __rmul__(NumericalComplex s) { return s * (*self); }
  ComplexMatrix __rmul__(const ComplexMatrix & m) { return m * (*self); }

  HermitianMatrix __truediv__(NumericalComplex s) { return (*self) / s; }

} // HermitianMatrix
} // OT
