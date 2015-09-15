// SWIG file TriangularComplexMatrix.i

%{
#include "TriangularComplexMatrix.hxx"
%}

%include TriangularComplexMatrix_doc.i

%include TriangularComplexMatrix.hxx

namespace OT { 

%extend TriangularComplexMatrix {

  TriangularComplexMatrix(const TriangularComplexMatrix & other) { return new OT::TriangularComplexMatrix(other); }

  TriangularComplexMatrix(PyObject * pyObj) { return new OT::TriangularComplexMatrix( OT::convert<OT::_PySequence_,OT::TriangularComplexMatrix>(pyObj) ); }

  OTComplexMatrixGetAccessors()  
    
  TriangularComplexMatrix __rmul__(NumericalComplex s) { return s * (*self); }
  
  TriangularComplexMatrix __truediv__(NumericalComplex s) { return (*self) / s; }
  
  } // TriangularComplexMatrix
} // OT
