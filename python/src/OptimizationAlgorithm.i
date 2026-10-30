// SWIG file OptimizationAlgorithm.i

%{
#include "openturns/OptimizationAlgorithm.hxx"
%}

%include OptimizationAlgorithm_doc.i

%ignore OT::OptimizationAlgorithm::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::OptimizationAlgorithm::setStopCallback(StopCallback callBack, void * data);

OTTypedInterfaceObjectHelper(OptimizationAlgorithm)

%copyctor OT::OptimizationAlgorithm;
%include openturns/OptimizationAlgorithm.hxx
namespace OT{ %extend OptimizationAlgorithm {

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    // Keep the callable alive: the C++ side stores it as void* without ownership,
    // so a temporary would be garbage-collected before it is invoked
    Py_XINCREF(callBack);
    self->setProgressCallback(&OptimizationAlgorithmImplementation_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    Py_XINCREF(callBack);
    self->setStopCallback(&OptimizationAlgorithmImplementation_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} }
