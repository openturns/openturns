// SWIG file GreaterOrEqual.i

%{
#include "GreaterOrEqual.hxx"
%}

%include GreaterOrEqual_doc.i

%include GreaterOrEqual.hxx
namespace OT {  
%extend GreaterOrEqual {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend GreaterOrEqual { GreaterOrEqual(const GreaterOrEqual & other) { return new OT::GreaterOrEqual(other); } } }
