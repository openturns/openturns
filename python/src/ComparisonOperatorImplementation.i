// SWIG file ComparisonOperatorImplementation.i

%{
#include "openturns/ComparisonOperatorImplementation.hxx"
%}

%include openturns/ComparisonOperatorImplementation.hxx
namespace OT { %extend ComparisonOperatorImplementation { ComparisonOperatorImplementation(const ComparisonOperatorImplementation & other) { return new OT::ComparisonOperatorImplementation(other); } } }
