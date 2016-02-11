// SWIG file FixedExperiment.i

%{
#include "openturns/FixedExperiment.hxx"
%}

%include FixedExperiment_doc.i

%include openturns/FixedExperiment.hxx
namespace OT { %extend FixedExperiment { FixedExperiment(const FixedExperiment & other) { return new OT::FixedExperiment(other); } } }
