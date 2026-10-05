// SWIG file Pointer.i

%{
#include "openturns/Pointer.hxx"
%}

%ignore OT::Pointer::operator=;
%ignore OT::Pointer::reset;
%ignore OT::Pointer::use_count;
%ignore OT::Pointer::swap;
%ignore OT::Pointer::assign;
%ignore OT::Pointer::unique;

%include openturns/Pointer.hxx
