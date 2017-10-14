// SWIG file MonteCarloExperiment.i

%{
#include "openturns/MonteCarloExperiment.hxx"
%}

%include MonteCarloExperiment_doc.i

%include openturns/MonteCarloExperiment.hxx
namespace OT { %extend MonteCarloExperiment { MonteCarloExperiment(const MonteCarloExperiment & other) { return new OT::MonteCarloExperiment(other); } } }
