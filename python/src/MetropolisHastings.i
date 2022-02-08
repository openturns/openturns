// SWIG file MetropolisHastings.i

%{
#include "openturns/MetropolisHastings.hxx"
%}

%include MetropolisHastings_doc.i

OTDefaultCollectionConvertFunctions(MetropolisHastings)

OTTypedInterfaceObjectHelper(MetropolisHastings)
OTTypedCollectionInterfaceObjectHelper(MetropolisHastings)

%include openturns/MetropolisHastings.hxx

namespace OT { %extend MetropolisHastings { MetropolisHastings(const MetropolisHastings & other) { return new OT::MetropolisHastings(other); } } }
