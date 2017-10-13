// SWIG file SquareMatrix.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::SquareComplexMatrix & v ($*ltype temp) %{ temp = OT::SquareComplexMatrix(); $1 = &temp; %}
%typemap(argout) OT::SquareComplexMatrix & v %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SquareComplexMatrix(*$1), SWIG_TypeQuery("OT::SquareComplexMatrix *"), SWIG_POINTER_OWN |  0 )); %}
OT::SquareMatrix OT::SquareMatrix::computeEV(OT::SquareComplexMatrix & v, const Bool keepIntact = true);

%{
#include "openturns/SquareMatrix.hxx"
%}

%include SquareMatrix_doc.i

%rename(__pow__) OT::SquareMatrix::power(const UnsignedInteger n) const;

%include openturns/SquareMatrix.hxx

namespace OT {  

%extend SquareMatrix {

  SquareMatrix(const SquareMatrix & other) { return new OT::SquareMatrix(other); }

  SquareMatrix(PyObject * pyObj) { return new OT::SquareMatrix( OT::convert<OT::_PySequence_,OT::SquareMatrix>(pyObj) ); }

  OTMatrixAccessors()

#if SWIG_VERSION < 0x030011
  SquareMatrix __truediv__(Scalar s) { return (*self) / s; }
#endif

} // SquareMatrix
} // OT

%clear OT::SquareComplexMatrix & v;
