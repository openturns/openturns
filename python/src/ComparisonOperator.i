// SWIG file ComparisonOperator.i

%{
#include "openturns/ComparisonOperator.hxx"
%}

%include ComparisonOperator_doc.i

OTTypedInterfaceObjectHelper(ComparisonOperator)

%include openturns/ComparisonOperator.hxx
namespace OT {  
%extend ComparisonOperator {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend ComparisonOperator { ComparisonOperator(const ComparisonOperator & other) { return new OT::ComparisonOperator(other); } } }
