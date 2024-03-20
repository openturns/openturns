// SWIG file ComparisonOperator.i

%{
#include "openturns/ComparisonOperator.hxx"
%}

%include ComparisonOperator_doc.i

OTTypedInterfaceObjectHelper(ComparisonOperator)

%copyctor OT::ComparisonOperator;

%include openturns/ComparisonOperator.hxx
