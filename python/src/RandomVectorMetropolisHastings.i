// SWIG file RandomVectorMetropolisHastings.i

%{
#include "openturns/RandomVectorMetropolisHastings.hxx"
%}

%include RandomVectorMetropolisHastings_doc.i

%include openturns/RandomVectorMetropolisHastings.hxx
namespace OT { %extend RandomVectorMetropolisHastings { RandomVectorMetropolisHastings(const RandomVectorMetropolisHastings & other) { return new OT::RandomVectorMetropolisHastings(other); } } }

