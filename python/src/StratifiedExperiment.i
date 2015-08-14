// SWIG file StratifiedExperiment.i

%{
#include "StratifiedExperiment.hxx"
%}

%include StratifiedExperiment_doc.i

%include StratifiedExperiment.hxx
namespace OT { %extend StratifiedExperiment { StratifiedExperiment(const StratifiedExperiment & other) { return new OT::StratifiedExperiment(other); } } }
