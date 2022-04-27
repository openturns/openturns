// SWIG file RandomWalkMetropolisHastings.i

%{
#include "openturns/RandomWalkMetropolisHastings.hxx"
%}

%include RandomWalkMetropolisHastings_doc.i

%include openturns/RandomWalkMetropolisHastings.hxx
namespace OT { %extend RandomWalkMetropolisHastings { RandomWalkMetropolisHastings(const RandomWalkMetropolisHastings & other) { return new OT::RandomWalkMetropolisHastings(other); } } }

