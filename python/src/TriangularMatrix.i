// SWIG file TriangularMatrix.i

%{
#include "openturns/TriangularMatrix.hxx"
%}

%include TriangularMatrix_doc.i

%include openturns/TriangularMatrix.hxx

namespace OT {

%extend TriangularMatrix {

  TriangularMatrix(const TriangularMatrix & other) { return new OT::TriangularMatrix(other); }

  TriangularMatrix(PyObject * pyObj) { return new OT::TriangularMatrix( OT::convert<OT::_PySequence_,OT::TriangularMatrix>(pyObj) ); }

  OTMatrixAccessors()

  TriangularMatrix __rmul__(NumericalScalar s) { return s * (*self); }
  Matrix __rmul__(const Matrix & m) { return m * (*self); }

  TriangularMatrix __truediv__(NumericalScalar s) { return (*self) / s; }

  } // TriangularMatrix
} // OT
