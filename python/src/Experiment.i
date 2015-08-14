// SWIG file Experiment.i

%{
#include "Experiment.hxx"
%}

%include Experiment_doc.i

OTTypedInterfaceObjectHelper(Experiment)

%include Experiment.hxx
namespace OT { %extend Experiment { Experiment(const Experiment & other) { return new OT::Experiment(other); } } }
