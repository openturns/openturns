// SWIG file QuantileSimulation.i

%{
#include "openturns/QuantileSimulation.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void QuantileSimulation_ProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, point.get(), NULL));
  if (result.isNull())
    OT::handleException();
}

static OT::Bool QuantileSimulation_StopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, NULL));
  if (result.isNull())
    OT::handleException();
  return OT::checkAndConvert< OT::_PyLong_, OT::UnsignedInteger >(result.get());
}

%}

%include QuantileSimulation_doc.i

%ignore OT::QuantileSimulation::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::QuantileSimulation::setStopCallback(StopCallback callBack, void * data);

OTTypedInterfaceObjectHelper(QuantileSimulation)

%copyctor OT::QuantileSimulation;

%include openturns/QuantileSimulation.hxx

namespace OT {

%extend QuantileSimulation {

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&QuantileSimulation_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&QuantileSimulation_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} // QuantileSimulation
} // OT
