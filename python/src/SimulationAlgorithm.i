// SWIG file SimulationAlgorithm.i

%{
#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void SimulationAlgorithm_ProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, point.get(), NULL));
  if (result.isNull())
    OT::handleException();
}

static OT::Bool SimulationAlgorithm_StopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, NULL));
  if (result.isNull())
    OT::handleException();
  return OT::checkAndConvert< OT::_PyInt_, OT::UnsignedInteger >(result.get());
}

%}

%include SimulationAlgorithm_doc.i

%ignore OT::SimulationAlgorithm::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::SimulationAlgorithm::setStopCallback(StopCallback callBack, void * data);

%include openturns/SimulationAlgorithm.hxx

namespace OT {


%extend SimulationAlgorithm {

SimulationAlgorithm(const SimulationAlgorithm & other) { return new OT::SimulationAlgorithm(other); }

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&SimulationAlgorithm_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&SimulationAlgorithm_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} // SimulationAlgorithm
} // OT

