// SWIG file Experiment.i

%{
#include "openturns/Experiment.hxx"
#include "openturns/PythonExperimentImplementation.hxx"

%}

%include Experiment_doc.i

OTTypedInterfaceObjectHelper(Experiment)

%include openturns/Experiment.hxx
namespace OT { %extend Experiment {

Experiment(const Experiment & other) { return new OT::Experiment(other); }

Experiment(PyObject * pyObj)
{
  return new OT::Experiment( new OT::PythonExperimentImplementation(pyObj) );
}


} }
