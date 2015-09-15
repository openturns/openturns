// SWIG file LessOrEqual.i

%{
#include "LessOrEqual.hxx"
%}

%include LessOrEqual_doc.i

%include LessOrEqual.hxx
namespace OT {  
%extend LessOrEqual {
  const String __str__() const { return self->__repr__(); }
}
}
namespace OT { %extend LessOrEqual { LessOrEqual(const LessOrEqual & other) { return new OT::LessOrEqual(other); } } }
