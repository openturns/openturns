// SWIG file MetropolisHastings.i

%{
#include "openturns/MetropolisHastings.hxx"
%}

%include MetropolisHastings_doc.i

OTDefaultCollectionConvertFunctions(MetropolisHastings)

OTTypedInterfaceObjectHelper(MetropolisHastings)
OTTypedCollectionInterfaceObjectHelper(MetropolisHastings)

%copyctor OT::MetropolisHastings;

%include openturns/MetropolisHastings.hxx
