// SWIG file SimulationAlgorithmImplementation.i

%{
#include "openturns/SimulationAlgorithmImplementation.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void SimulationAlgorithmImplementation_ProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, point.get(), NULL));
  if (result.isNull())
    OT::handleException();
}

static OT::Bool SimulationAlgorithmImplementation_StopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, NULL));
  if (result.isNull())
    OT::handleException();
  return OT::checkAndConvert< OT::_PyLong_, OT::UnsignedInteger >(result.get());
}

%}

%include SimulationAlgorithmImplementation_doc.i

%ignore OT::SimulationAlgorithmImplementation::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::SimulationAlgorithmImplementation::setStopCallback(StopCallback callBack, void * data);

%copyctor OT::SimulationAlgorithmImplementation;

%include openturns/SimulationAlgorithmImplementation.hxx

namespace OT {

%extend SimulationAlgorithmImplementation {

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&SimulationAlgorithmImplementation_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&SimulationAlgorithmImplementation_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} // SimulationAlgorithmImplementation
} // OT
