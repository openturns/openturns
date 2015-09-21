// SWIG file Equal.i

%{
#include "Equal.hxx"
%}

%include Equal_doc.i

%include Equal.hxx
namespace OT {  
%extend Equal {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend Equal { Equal(const Equal & other) { return new OT::Equal(other); } } }
