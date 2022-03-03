// SWIG file TriangularMatrix.i

%{
#include "openturns/TriangularMatrix.hxx"
%}

%include TriangularMatrix_doc.i

%ignore OT::TriangularMatrix::operator * (const ScalarCollection &) const;

%include openturns/TriangularMatrix.hxx

namespace OT {

%extend TriangularMatrix {

  TriangularMatrix(const TriangularMatrix & other) { return new OT::TriangularMatrix(other); }

  TriangularMatrix(PyObject * pyObj) { return new OT::TriangularMatrix( OT::convert<OT::_PySequence_,OT::TriangularMatrix>(pyObj) ); }

  OTMatrixAccessors()

  TriangularMatrix __rmul__(Scalar s) { return s * (*self); }
  Matrix __rmul__(const Matrix & m) { return m * (*self); }

#if SWIG_VERSION < 0x030011
  TriangularMatrix __truediv__(Scalar s) { return (*self) / s; }
#endif

  } // TriangularMatrix
} // OT
