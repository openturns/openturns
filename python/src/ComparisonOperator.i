// SWIG file ComparisonOperator.i

%{
#include "openturns/ComparisonOperator.hxx"
%}

%include ComparisonOperator_doc.i

OTTypedInterfaceObjectHelper(ComparisonOperator)

%include openturns/ComparisonOperator.hxx

namespace OT { %extend ComparisonOperator { ComparisonOperator(const ComparisonOperator & other) { return new OT::ComparisonOperator(other); } } }
