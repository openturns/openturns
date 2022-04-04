// SWIG file IndependentMetropolisHastings.i

%{
#include "openturns/IndependentMetropolisHastings.hxx"
%}

%include IndependentMetropolisHastings_doc.i

%include openturns/IndependentMetropolisHastings.hxx
namespace OT { %extend IndependentMetropolisHastings { IndependentMetropolisHastings(const IndependentMetropolisHastings & other) { return new OT::IndependentMetropolisHastings(other); } } }

