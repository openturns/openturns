// SWIG file EventSimulation.i

%{
#include "openturns/EventSimulation.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void EventSimulation_ProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, point.get(), NULL));
  if (result.isNull())
    OT::handleException();
}

static OT::Bool EventSimulation_StopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, NULL));
  if (result.isNull())
    OT::handleException();
  return OT::checkAndConvert< OT::_PyLong_, OT::UnsignedInteger >(result.get());
}

%}

%include EventSimulation_doc.i

%ignore OT::EventSimulation::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::EventSimulation::setStopCallback(StopCallback callBack, void * data);

OTTypedInterfaceObjectHelper(EventSimulation)

%copyctor OT::EventSimulation;
%include openturns/EventSimulation.hxx

namespace OT {

%extend EventSimulation {

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&EventSimulation_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&EventSimulation_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} // EventSimulation
} // OT
