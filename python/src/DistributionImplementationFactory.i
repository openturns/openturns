// SWIG file DistributionImplementationFactory.i

%{
#include "DistributionImplementationFactory.hxx"
%}

%include DistributionImplementationFactory_doc.i

%include DistributionImplementationFactory.hxx
namespace OT {  

%extend DistributionImplementationFactory {

DistributionImplementationFactory(const DistributionImplementationFactory & other)
{
  return new OT::DistributionImplementationFactory(other);
}

}
}
