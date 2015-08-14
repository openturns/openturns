// SWIG file DistributionParametersImplementation.i

%{
#include "DistributionParametersImplementation.hxx"
%}

%include DistributionParametersImplementation_doc.i

%include DistributionParametersImplementation.hxx
namespace OT { %extend DistributionParametersImplementation { DistributionParametersImplementation(const DistributionParametersImplementation & other) { return new OT::DistributionParametersImplementation(other); } } }
