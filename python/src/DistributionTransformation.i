// SWIG file DistributionTransformation.i

%{
#include "openturns/DistributionTransformation.hxx"
%}

%include DistributionTransformation_doc.i

%include openturns/DistributionTransformation.hxx
namespace OT { %extend DistributionTransformation { DistributionTransformation(const DistributionTransformation & other) { return new OT::DistributionTransformation(other); } } }
