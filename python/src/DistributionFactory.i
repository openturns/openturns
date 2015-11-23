// SWIG file DistributionFactory.i

%{
#include "DistributionFactory.hxx"
%}

%include DistributionFactory_doc.i

%include UncertaintyModelCopulaCollection.i

OTTypedInterfaceObjectHelper(DistributionFactory)
OTTypedCollectionInterfaceObjectHelper(DistributionFactory)

%include DistributionFactory.hxx

namespace OT {

%extend DistributionFactory {

DistributionFactory(const DistributionFactory & other) { return new OT::DistributionFactory(other); }

}
}
