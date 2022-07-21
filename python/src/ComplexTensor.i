// SWIG file ComplexTensor.i

%{
#include "openturns/ComplexTensor.hxx"
%}

%include ComplexTensor_doc.i

%template(ComplexTensorImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::ComplexTensorImplementation>;
%apply const ComplexCollection & { const OT::ComplexTensor::ComplexCollection & };

%rename(__baseaddress__) OT::ComplexTensor::data;
%rename(__elementsize__) OT::ComplexTensor::elementSize;
%rename(__stride__) OT::ComplexTensor::stride;

%include openturns/ComplexTensor.hxx

%pythoncode %{
# This code has been added to conform to Numpy ndarray interface
# that tries to reuse the data stored in the ComplexTensor (zero copy)
# see http://docs.scipy.org/doc/numpy/reference/arrays.interface.html#arrays-interface
# for details.
# See python doc http://docs.python.org/reference/datamodel.html?highlight=getattribute#object.__getattribute__
# for details on how to write such a method.
def ComplexTensor___getattribute__(self, name):
    """Implement attribute accesses."""
    if (name == '__array_interface__'):
        self.__dict__['__array_interface__'] = {'shape': (self.getNbRows(), self.getNbColumns(), self.getNbSheets()),
                                                'typestr': "|c" + str(self.__elementsize__()),
                                                'data': (int(self.__baseaddress__() or 1), True),
                                                'strides': (self.__stride__(0), self.__stride__(1), self.__stride__(2)),
                                                'version': 3,
                                                }
    return object.__getattribute__(self, name)
ComplexTensor.__getattribute__ = ComplexTensor___getattribute__
%}

namespace OT {

%extend ComplexTensor {

  ComplexTensor(const ComplexTensor & other) { return new OT::ComplexTensor(other); }

  ComplexTensor(PyObject * pyObj) { return new OT::ComplexTensor( OT::convert<OT::_PySequence_,OT::ComplexTensor>(pyObj) ); }

  OTTensorAccessors(ComplexTensor, Complex, _PyComplex_)

} // ComplexTensor
} // OT
