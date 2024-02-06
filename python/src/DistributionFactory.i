// SWIG file DistributionFactory.i

%{
#include "openturns/DistributionFactory.hxx"
%}

%include DistributionFactory_doc.i

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(DistributionFactory)
OTTypedCollectionInterfaceObjectHelper(DistributionFactory)

%include openturns/DistributionFactory.hxx

%copyctor OT::DistributionFactory;
