// SWIG file LowDiscrepancyExperiment.i

%{
#include "LowDiscrepancyExperiment.hxx"
%}

%include LowDiscrepancyExperiment_doc.i

%include LowDiscrepancyExperiment.hxx
namespace OT { %extend LowDiscrepancyExperiment { LowDiscrepancyExperiment(const LowDiscrepancyExperiment & other) { return new OT::LowDiscrepancyExperiment(other); } } }
