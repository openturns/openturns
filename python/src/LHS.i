// SWIG file LHS.i

%{
#include "LHS.hxx"
%}

%include LHS_doc.i

%include LHS.hxx
namespace OT{ %extend LHS { LHS(const LHS & other) { return new OT::LHS(other); } } }
