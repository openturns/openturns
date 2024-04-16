// SWIG file ExperimentIntegration.i

%{
#include "openturns/ExperimentIntegration.hxx"
%}

%include ExperimentIntegration_doc.i

%include openturns/ExperimentIntegration.hxx
namespace OT { %extend ExperimentIntegration { ExperimentIntegration(const ExperimentIntegration & other) { return new OT::ExperimentIntegration(other); } } }
