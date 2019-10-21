// SWIG file HermitianMatrix.i

%{
#include "openturns/HermitianMatrix.hxx"
%}

%include HermitianMatrix_doc.i

%include openturns/HermitianMatrix.hxx

%pythoncode %{
def HermitianMatrix___getattribute__(self, name):
    """Implement attribute accesses."""
    if name == '__array_interface__':
        self.checkHermitian()
    return super(HermitianMatrix, self).__getattribute__(name)
HermitianMatrix.__getattribute__ = HermitianMatrix___getattribute__
%}

namespace OT {

%extend HermitianMatrix {
  HermitianMatrix(const HermitianMatrix & other) { return new  OT::HermitianMatrix(other); }

  HermitianMatrix(PyObject * pyObj) { return new OT::HermitianMatrix( OT::convert<OT::_PySequence_,OT::HermitianMatrix>(pyObj) ); }

  OTComplexMatrixGetAccessors()

  HermitianMatrix __rmul__(Complex s) { return s * (*self); }
  ComplexMatrix __rmul__(const ComplexMatrix & m) { return m * (*self); }

#if SWIG_VERSION < 0x030011
  HermitianMatrix __truediv__(Complex s) { return (*self) / s; }
#endif

} // HermitianMatrix
} // OT
