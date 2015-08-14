// SWIG file BootstrapExperiment.i

%{
#include "BootstrapExperiment.hxx"
%}

%include BootstrapExperiment_doc.i

%include BootstrapExperiment.hxx
namespace OT { %extend BootstrapExperiment { BootstrapExperiment(const BootstrapExperiment & other) { return new OT::BootstrapExperiment(other); } } }
