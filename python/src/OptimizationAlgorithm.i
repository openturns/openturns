// SWIG file OptimizationAlgorithm.i

%{
#include "openturns/OptimizationAlgorithm.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void OptimizationAlgorithm_ProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, point.get(), NULL ));
}

static OT::Bool OptimizationAlgorithm_StopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, NULL ));
  return OT::convert< OT::_PyInt_, OT::UnsignedInteger >(result.get());
}
%}

%include OptimizationAlgorithm_doc.i

%ignore OT::OptimizationAlgorithm::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::OptimizationAlgorithm::setStopCallback(StopCallback callBack, void * data);

OTTypedInterfaceObjectHelper(OptimizationAlgorithm)

%include openturns/OptimizationAlgorithm.hxx
namespace OT{ %extend OptimizationAlgorithm {

OptimizationAlgorithm(const OptimizationAlgorithm & other) { return new OT::OptimizationAlgorithm(other); }


void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&OptimizationAlgorithm_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&OptimizationAlgorithm_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} }
