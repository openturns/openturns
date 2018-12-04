// SWIG file OptimizationAlgorithm.i

%{
#include "openturns/OptimizationAlgorithm.hxx"
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
    self->setProgressCallback(&OptimizationAlgorithmImplementation_ProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&OptimizationAlgorithmImplementation_StopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} }
