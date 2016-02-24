// SWIG file DistributionFactory.i

%{
#include "openturns/DistributionFactory.hxx"
%}

%include DistributionFactory_doc.i

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(DistributionFactory)
OTTypedCollectionInterfaceObjectHelper(DistributionFactory)

%include openturns/DistributionFactory.hxx

namespace OT {

%extend DistributionFactory {

DistributionFactory(const DistributionFactory & other) { return new OT::DistributionFactory(other); }

}
}
