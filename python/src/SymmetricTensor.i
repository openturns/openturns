// SWIG file SymmetricTensor.i

%{
#include "SymmetricTensor.hxx"
%}

%include SymmetricTensor_doc.i

%apply const NumericalScalarCollection & { const OT::SymmetricTensor::NumericalScalarCollection & };

%include SymmetricTensor.hxx

namespace OT {  

%extend SymmetricTensor {

  SymmetricTensor(const SymmetricTensor & other) { return new OT::SymmetricTensor(other); }

  SymmetricTensor(PyObject * pyObj) { return new OT::SymmetricTensor( OT::convert<OT::_PySequence_,OT::SymmetricTensor>(pyObj) ); }

  OT_TENSOR_ACCESSORS(SymmetricTensor)

} // SymmetricTensor
} // OT
