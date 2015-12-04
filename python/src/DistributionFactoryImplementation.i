// SWIG file DistributionFactoryImplementation.i

%{
#include "DistributionFactoryImplementation.hxx"
%}

%include DistributionFactoryImplementation_doc.i

%include DistributionFactoryImplementation.hxx
namespace OT {  

%extend DistributionFactoryImplementation {

DistributionFactoryImplementation(const DistributionFactoryImplementation & other)
{
  return new OT::DistributionFactoryImplementation(other);
}

}
}
