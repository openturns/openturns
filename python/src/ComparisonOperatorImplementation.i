// SWIG file ComparisonOperatorImplementation.i

%{
#include "ComparisonOperatorImplementation.hxx"
%}

%include ComparisonOperatorImplementation.hxx
namespace OT { %extend ComparisonOperatorImplementation { ComparisonOperatorImplementation(const ComparisonOperatorImplementation & other) { return new OT::ComparisonOperatorImplementation(other); } } }
