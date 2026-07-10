// SWIG file DistributionFactory.i

%{
#include "openturns/DistributionFactory.hxx"
%}

%include DistributionFactory_doc.i

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(DistributionFactory)
OTTypedCollectionInterfaceObjectHelper(DistributionFactory)

%copyctor OT::DistributionFactory;
%include openturns/DistributionFactory.hxx

