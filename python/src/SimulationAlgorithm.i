// SWIG file SimulationAlgorithm.i

%{
#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void PythonProgressCallback(OT::Scalar percent, void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, point.get(), NULL ));
}

static OT::Bool PythonStopCallback(void * data) {
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs( pyObj, NULL ));
  return OT::convert< OT::_PyInt_, OT::UnsignedInteger >(result.get());
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

} // SimulationAlgorithm
} // OT


%pythoncode %{
def TimerCallback(duration):
    """
    Time stopping criterion.

    Usable as stopping callback for simulation, optimization algorithms.

    Parameters
    ----------
    duration : float
        Maximum duration in seconds of the algorithm.

    Examples
    --------
    >>> import openturns as ot
    >>> model = ot.SymbolicFunction(['R', 'S'], ['R-S'])
    >>> distribution = ot.Normal(2)
    >>> vect = ot.RandomVector(distribution)
    >>> output = ot.RandomVector(model, vect)
    >>> event = ot.Event(output, ot.Less(), 0.0)
    >>> experiment = ot.MonteCarloExperiment()
    >>> algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
    >>> algo.setMaximumOuterSampling(int(1e9))
    >>> algo.setMaximumCoefficientOfVariation(-1.0)
    >>> algo.setStopCallback(TimerCallback(1.5))
    >>> algo.run()
    """
    from time import time
    try:
        float(duration)
    except ValueError:
        raise ValueError('duration must be a float')
    tmax = time() + duration
    def inner():
        if not hasattr(inner, 'tmax'):
            inner.tmax = tmax
        stop = time() > inner.tmax
        return stop
    return inner
%}
