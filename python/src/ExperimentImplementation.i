// SWIG file ExperimentImplementation.i

%{
#include "ExperimentImplementation.hxx"
%}

%include ExperimentImplementation_doc.i

%include ExperimentImplementation.hxx
namespace OT { %extend ExperimentImplementation { ExperimentImplementation(const ExperimentImplementation & other) { return new OT::ExperimentImplementation(other); } } }
