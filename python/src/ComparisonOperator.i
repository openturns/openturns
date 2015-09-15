// SWIG file ComparisonOperator.i

%{
#include "ComparisonOperator.hxx"
%}

%include ComparisonOperator_doc.i

OTTypedInterfaceObjectHelper(ComparisonOperator)

%include ComparisonOperator.hxx
namespace OT {  
%extend ComparisonOperator {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend ComparisonOperator { ComparisonOperator(const ComparisonOperator & other) { return new OT::ComparisonOperator(other); } } }
