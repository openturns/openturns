// SWIG file ComplexMatrix.i

%{
#include "openturns/ComplexMatrix.hxx"
%}

%include ComplexMatrix_doc.i

%template(ComplexMatrixImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::ComplexMatrixImplementation>;

%template(ComplexCollection)                   OT::Collection<OT::Complex>;
%template(ComplexPersistenCollection)          OT::PersistentCollection<OT::Complex>;

%typemap(in) const ComplexCollection & ($1_basetype temp) {
  if (! SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, $1_descriptor, SWIG_POINTER_NO_NULL))) {
    try {
      temp = OT::convert<OT::_PySequence_,OT::Collection<OT::Complex> >( $input );
      $1 = &temp;
    } catch (const OT::InvalidArgumentException &) {
      SWIG_exception(SWIG_TypeError, "Object passed as argument is not convertible to a collection of Complex");
    }
  }
}

%typemap(typecheck,precedence=SWIG_TYPECHECK_POINTER) const ComplexCollection & {
  $1 = SWIG_IsOK(SWIG_ConvertPtr($input, NULL, $1_descriptor, SWIG_POINTER_NO_NULL)) ||
       OT::isAPythonBufferOf<OT::Complex, 2>( $input ) || OT::isAPythonSequenceOf<OT::_PyComplex_>($input);
}
%apply const ScalarCollection & { const OT::ComplexMatrix::ScalarCollection & };
%apply const ComplexCollection & { const OT::ComplexMatrix::ComplexCollection & };

%rename(__baseaddress__) OT::ComplexMatrix::data;
%rename(__elementsize__) OT::ComplexMatrix::elementSize;
%rename(__stride__) OT::ComplexMatrix::stride;

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
    if name == '__array_interface__':
        self.__dict__['__array_interface__'] = {'shape': (self.getNbRows(), self.getNbColumns()),
                                                'typestr': "|c" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__() or 1), True),
                                                'strides': (self.__stride__(0), self.__stride__(1)),
                                                'version': 3,
                                                }
    return super(ComplexMatrix, self).__getattribute__(name)
ComplexMatrix.__getattribute__ = ComplexMatrix___getattribute__
%}


%define OTComplexMatrixGetAccessors()
  OTMatrixGetAccessor(ComplexMatrix, Complex, _PyComplex_)
  OTMatrixSetAccessor(ComplexMatrix, Complex, _PyComplex_)
%enddef

namespace OT {

%extend ComplexMatrix {

  ComplexMatrix(const ComplexMatrix & other) { return new OT::ComplexMatrix(other); }

  ComplexMatrix(PyObject * pyObj) { return new OT::ComplexMatrix( OT::convert<OT::_PySequence_,OT::ComplexMatrix>(pyObj) ); }

  OTComplexMatrixGetAccessors()  
  
  ComplexMatrix __rmul__(Complex s) { return s * (*self); }

#if SWIG_VERSION < 0x030011
  ComplexMatrix __truediv__(Complex s) { return (*self) / s; }
#endif

  ComplexMatrix __matmul__(const ComplexMatrix & other) { return *self * other; }

  ComplexMatrix __matmul__(const Matrix & other) { return *self * other; }

} // ComplexMatrix
} // OT
