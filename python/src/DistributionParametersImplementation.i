// SWIG file DistributionParametersImplementation.i

%{
#include "openturns/DistributionParametersImplementation.hxx"
%}

%include DistributionParametersImplementation_doc.i

%include openturns/DistributionParametersImplementation.hxx
namespace OT { %extend DistributionParametersImplementation { DistributionParametersImplementation(const DistributionParametersImplementation & other) { return new OT::DistributionParametersImplementation(other); } } }
