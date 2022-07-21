// SWIG file MetropolisHastingsImplementation.i

%{
#include "openturns/MetropolisHastingsImplementation.hxx"
%}

%include MetropolisHastingsImplementation_doc.i

%include openturns/MetropolisHastingsImplementation.hxx
namespace OT { %extend MetropolisHastingsImplementation { MetropolisHastingsImplementation(const MetropolisHastingsImplementation & other) { return new OT::MetropolisHastingsImplementation(other); } } }

