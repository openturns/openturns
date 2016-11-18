// SWIG file

%{
#include "openturns/OptimalLHSExperiment.hxx"
%}

%include OptimalLHSExperiment_doc.i

%include openturns/OptimalLHSExperiment.hxx
namespace OT { %extend OptimalLHSExperiment { OptimalLHSExperiment(const OptimalLHSExperiment & other) { return new OT::OptimalLHSExperiment(other); } } }
