// SWIG file Simulation.i

%{
#include "openturns/Simulation.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void PythonProgressCallback(OT::NumericalScalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::NumericalScalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, point.get(), NULL ));
}

static OT::Bool PythonStopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, NULL ));
  return OT::convert< OT::_PyInt_, OT::UnsignedInteger >(result.get());
}

%}

%include Simulation_doc.i

%ignore OT::Simulation::setProgressCallback(ProgressCallback callBack, void * data);
%ignore OT::Simulation::setStopCallback(StopCallback callBack, void * data);

%include openturns/Simulation.hxx

namespace OT {


%extend Simulation {

Simulation(const Simulation & other) { return new OT::Simulation(other); }

void setProgressCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setProgressCallback(&PythonProgressCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

void setStopCallback(PyObject * callBack) {
  if (PyCallable_Check(callBack)) {
    self->setStopCallback(&PythonStopCallback, callBack);
  }
  else {
    throw OT::InvalidArgumentException(HERE) << "Argument is not a callable object.";
  }
}

} // Simulation
} // OT
