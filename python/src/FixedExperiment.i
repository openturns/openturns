// SWIG file FixedExperiment.i

%{
#include "FixedExperiment.hxx"
%}

%include FixedExperiment_doc.i

%include FixedExperiment.hxx
namespace OT { %extend FixedExperiment { FixedExperiment(const FixedExperiment & other) { return new OT::FixedExperiment(other); } } }
