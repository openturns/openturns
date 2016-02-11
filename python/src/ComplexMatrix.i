// SWIG file ComplexMatrix.i

%{
#include "openturns/ComplexMatrix.hxx"
%}

%include ComplexMatrix_doc.i

%template(ComplexMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::ComplexMatrixImplementation>;

%apply const NumericalScalarCollection & { const OT::ComplexMatrix::NumericalScalarCollection & };
%apply const NumericalComplexCollection & { const OT::ComplexMatrix::NumericalComplexCollection & };

%include openturns/ComplexMatrix.hxx

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the ComplexMatrix (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def ComplexMatrix___getattribute__(self, name):
    """Implement attribute accesses."""
    if (name == '__array_interface__'):
        self.__dict__['__array_interface__'] = {'shape': (self.getNbRows(), self.getNbColumns()),
                                                'typestr': "|c" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__()), True),
                                                'strides': (self.__stride__(0), self.__stride__(1)),
                                                'version': 3,
                                                }
    return object.__getattribute__(self, name)
ComplexMatrix.__getattribute__ = ComplexMatrix___getattribute__
%}


%define OTComplexMatrixGetAccessors()
  OTMatrixGetAccessor(ComplexMatrix, NumericalComplex, _PyComplex_)
  OTMatrixSetAccessor(ComplexMatrix, NumericalComplex, _PyComplex_)
%enddef    

namespace OT {

%extend ComplexMatrix {

  ComplexMatrix(const ComplexMatrix & other) { return new OT::ComplexMatrix(other); }

  ComplexMatrix(PyObject * pyObj) { return new OT::ComplexMatrix( OT::convert<OT::_PySequence_,OT::ComplexMatrix>(pyObj) ); }

  OTComplexMatrixGetAccessors()  
  
  ComplexMatrix __rmul__(NumericalComplex s) { return s * (*self); }
  
  ComplexMatrix __truediv__(NumericalComplex s) { return (*self) / s; }
  
  ComplexMatrix __matmul__(const ComplexMatrix & other) { return *self * other; }

  ComplexMatrix __matmul__(const Matrix & other) { return *self * other; }

} // ComplexMatrix
} // OT
