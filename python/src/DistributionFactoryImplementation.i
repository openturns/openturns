// SWIG file DistributionFactoryImplementation.i

%{
#include "openturns/DistributionFactoryImplementation.hxx"
%}

%include DistributionFactoryImplementation_doc.i

%include openturns/DistributionFactoryImplementation.hxx
namespace OT {  

%extend DistributionFactoryImplementation {

DistributionFactoryImplementation(const DistributionFactoryImplementation & other)
{
  return new OT::DistributionFactoryImplementation(other);
}

}
}
