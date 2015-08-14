// SWIG file ComplexTensor.i

%{
#include "ComplexTensor.hxx"
%}

%include ComplexTensor_doc.i

%template(ComplexTensorImplementationTypedInterfaceObject) OT::TypedInterfaceObject<OT::ComplexTensorImplementation>;
%apply const NumericalComplexCollection & { const OT::ComplexTensor::NumericalComplexCollection & };

%define OT_COMPLEXTENSOR_ACCESSORS(tensorType)

NumericalComplex __getitem__(PyObject * args) const {

  OT::UnsignedInteger arg2 ;
  OT::UnsignedInteger arg3 ;
  OT::UnsignedInteger arg4 ;
  unsigned long val2 ;
  int ecode2 = 0 ;
  unsigned long val3 ;
  int ecode3 = 0 ;
  unsigned long val4 ;
  int ecode4 = 0 ;
  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;
  PyObject * obj3 = 0 ;

  if (!PyArg_ParseTuple(args,(char *)"OOO:" #tensorType "___getitem__",&obj1,&obj2,&obj3)) SWIG_fail;

  ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
  if (!SWIG_IsOK(ecode2)) {
    SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #tensorType "___getitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
  }
  arg2 = static_cast< OT::UnsignedInteger >(val2);


  ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
  if (!SWIG_IsOK(ecode3)) {
    SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" #tensorType "___getitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
  }
  arg3 = static_cast< OT::UnsignedInteger >(val3);


  ecode4 = SWIG_AsVal_unsigned_SS_long(obj3, &val4);
  if (!SWIG_IsOK(ecode4)) {
    SWIG_exception_fail(SWIG_ArgError(ecode4), "in method '" #tensorType "___getitem__" "', argument " "4"" of type '" "OT::UnsignedInteger""'");
  }
  arg4 = static_cast< OT::UnsignedInteger >(val4);

  return (*self)(arg2,arg3,arg4);
fail:
  return 0.;
}

void __setitem__(PyObject * args, NumericalComplex val) {

  OT::UnsignedInteger arg2 ;
  OT::UnsignedInteger arg3 ;
  OT::UnsignedInteger arg4 ;
  unsigned long val2 ;
  int ecode2 = 0 ;
  unsigned long val3 ;
  int ecode3 = 0 ;
  unsigned long val4 ;
  int ecode4 = 0 ;
  PyObject * obj1 = 0 ;
  PyObject * obj2 = 0 ;
  PyObject * obj3 = 0 ;

  if (!PyArg_ParseTuple(args,(char *)"OOO:" #tensorType "___setitem__",&obj1,&obj2,&obj3)) SWIG_fail;

  ecode2 = SWIG_AsVal_unsigned_SS_long(obj1, &val2);
  if (!SWIG_IsOK(ecode2)) {
    SWIG_exception_fail(SWIG_ArgError(ecode2), "in method '" #tensorType "___setitem__" "', argument " "2"" of type '" "OT::UnsignedInteger""'");
  }
  arg2 = static_cast< OT::UnsignedInteger >(val2);


  ecode3 = SWIG_AsVal_unsigned_SS_long(obj2, &val3);
  if (!SWIG_IsOK(ecode3)) {
    SWIG_exception_fail(SWIG_ArgError(ecode3), "in method '" #tensorType "___setitem__" "', argument " "3"" of type '" "OT::UnsignedInteger""'");
  }
  arg3 = static_cast< OT::UnsignedInteger >(val3);


  ecode4 = SWIG_AsVal_unsigned_SS_long(obj3, &val4);
  if (!SWIG_IsOK(ecode4)) {
    SWIG_exception_fail(SWIG_ArgError(ecode4), "in method '" #tensorType "___setitem__" "', argument " "4"" of type '" "OT::UnsignedInteger""'");
  }
  arg4 = static_cast< OT::UnsignedInteger >(val4);

  (*self)(arg2,arg3,arg4) = val;
fail:
  return;
}
%enddef


%include ComplexTensor.hxx

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
                                                'data': (int(self.__baseaddress__()), True),
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

  OT_COMPLEXTENSOR_ACCESSORS(ComplexTensor)

} // ComplexTensor
} // OT
