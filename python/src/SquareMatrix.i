// SWIG file SquareMatrix.i

// do not pass argument by reference, return it as tuple item
%typemap(in, numinputs=0) OT::SquareComplexMatrix & vOut ($*ltype temp) %{ temp = OT::SquareComplexMatrix(); $1 = &temp; %}
%typemap(argout) OT::SquareComplexMatrix & vOut %{ $result = SWIG_Python_AppendOutput($result, SWIG_NewPointerObj(new OT::SquareComplexMatrix(*$1), SWIG_TypeQuery("OT::SquareComplexMatrix *"), SWIG_POINTER_OWN |  0 )); %}

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

} // SquareMatrix
} // OT
