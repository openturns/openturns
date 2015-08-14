// SWIG file RandomWalkMetropolisHastings.i

%{
#include "RandomWalkMetropolisHastings.hxx"
%}

%include RandomWalkMetropolisHastings_doc.i

%include RandomWalkMetropolisHastings.hxx
namespace OT { %extend RandomWalkMetropolisHastings { RandomWalkMetropolisHastings(const RandomWalkMetropolisHastings & other) { return new OT::RandomWalkMetropolisHastings(other); } } }
