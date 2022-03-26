// SWIG file SmolyakExperiment.i

%{
#include "openturns/SmolyakExperiment.hxx"
%}

%include SmolyakExperiment_doc.i

%include openturns/SmolyakExperiment.hxx
namespace OT { %extend SmolyakExperiment { SmolyakExperiment(const SmolyakExperiment & other) { return new OT::SmolyakExperiment(other); } } }
