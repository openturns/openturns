// SWIG file ExperimentImplementation.i

%{
#include "openturns/ExperimentImplementation.hxx"
%}

%include ExperimentImplementation_doc.i

%include openturns/ExperimentImplementation.hxx
namespace OT { %extend ExperimentImplementation { ExperimentImplementation(const ExperimentImplementation & other) { return new OT::ExperimentImplementation(other); } } }
