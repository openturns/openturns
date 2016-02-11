// SWIG file StratifiedExperiment.i

%{
#include "openturns/StratifiedExperiment.hxx"
%}

%include StratifiedExperiment_doc.i

%include openturns/StratifiedExperiment.hxx
namespace OT { %extend StratifiedExperiment { StratifiedExperiment(const StratifiedExperiment & other) { return new OT::StratifiedExperiment(other); } } }
