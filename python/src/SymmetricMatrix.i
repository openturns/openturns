// SWIG file SymmetricMatrix.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::SquareMatrix & v ($*ltype temp) %{ temp = OT::SquareMatrix(); $1 = &temp; %}
%typemap(argout) OT::SquareMatrix & v %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SquareMatrix(*$1), SWIG_TypeQuery("OT::SquareMatrix *"), SWIG_POINTER_OWN |  0 )); %}
OT::SymmetricMatrix OT::SymmetricMatrix::computeEV(OT::SquareMatrix & v, const Bool keepIntact = true);

%{
#include "SymmetricMatrix.hxx"
%}

%include SymmetricMatrix_doc.i

%ignore OT::SymmetricMatrix::checkSymmetry;

%apply const NumericalScalarCollection & { const OT::SymmetricMatrix::NumericalScalarCollection & };

%include SymmetricMatrix.hxx

namespace OT {

%extend SymmetricMatrix {

  SymmetricMatrix(const SymmetricMatrix & other) { return new OT::SymmetricMatrix(other); }

  SymmetricMatrix(PyObject * pyObj) { return new OT::SymmetricMatrix( OT::convert<OT::_PySequence_,OT::SymmetricMatrix>(pyObj) ); }

  OTMatrixAccessors()

  SymmetricMatrix __rmul__(NumericalScalar s) { return s * (*self); }
  Matrix __rmul__(const Matrix & m) { return m * (*self); }

  SymmetricMatrix __truediv__(NumericalScalar s) { return (*self) / s; }

} // SymmetricMatrix
} // OT
