// SWIG file MonteCarloExperiment.i

%{
#include "MonteCarloExperiment.hxx"
%}

%include MonteCarloExperiment_doc.i

%include MonteCarloExperiment.hxx
namespace OT { %extend MonteCarloExperiment { MonteCarloExperiment(const MonteCarloExperiment & other) { return new OT::MonteCarloExperiment(other); } } }
