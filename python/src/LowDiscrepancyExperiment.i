// SWIG file LowDiscrepancyExperiment.i

%{
#include "openturns/LowDiscrepancyExperiment.hxx"
%}

%include LowDiscrepancyExperiment_doc.i

%include openturns/LowDiscrepancyExperiment.hxx
namespace OT { %extend LowDiscrepancyExperiment { LowDiscrepancyExperiment(const LowDiscrepancyExperiment & other) { return new OT::LowDiscrepancyExperiment(other); } } }
