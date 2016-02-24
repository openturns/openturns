// SWIG file BootstrapExperiment.i

%{
#include "openturns/BootstrapExperiment.hxx"
%}

%include BootstrapExperiment_doc.i

%include openturns/BootstrapExperiment.hxx
namespace OT { %extend BootstrapExperiment { BootstrapExperiment(const BootstrapExperiment & other) { return new OT::BootstrapExperiment(other); } } }
