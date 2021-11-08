// SWIG file SimulationAlgorithm.i

%{
#include "openturns/SimulationAlgorithm.hxx"
#include "openturns/PythonWrappingFunctions.hxx"

static void SimulationAlgorithm_ProgressCallback(OT::Scalar percent, void * data) {
  OT::InterpreterUnlocker iul;
  PyObject * pyObj = reinterpret_cast<PyObject *>(data);
  OT::ScopedPyObjectPointer point(OT::convert< OT::Scalar, OT::_PyFloat_ >(percent));
  OT::ScopedPyObjectPointer result(PyObject_CallFunctionObjArgs(pyObj, point.get(), NULL));
  if (result.isNull())
    OT::handleException();
}

static OT::Bool SimulationAlgorithm_StopCallback(void * data) {
  OT::InterpreterUnlocker iul;
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
    >>> output = ot.CompositeRandomVector(model, vect)
    >>> event = ot.ThresholdEvent(output, ot.Less(), 0.0)
    >>> experiment = ot.MonteCarloExperiment()
    >>> algo = ot.ProbabilitySimulationAlgorithm(event, experiment)
    >>> algo.setMaximumOuterSampling(int(1e9))
    >>> algo.setMaximumCoefficientOfVariation(-1.0)
    >>> timer = TimerCallback(1.5)
    >>> algo.setStopCallback(timer)
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
