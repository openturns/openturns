// SWIG file Experiment.i

%{
#include "openturns/Experiment.hxx"
%}

%include Experiment_doc.i

OTTypedInterfaceObjectHelper(Experiment)

%include openturns/Experiment.hxx
namespace OT { %extend Experiment { Experiment(const Experiment & other) { return new OT::Experiment(other); } } }
