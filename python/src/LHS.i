// SWIG file LHS.i

%{
#include "openturns/LHS.hxx"
%}

%include LHS_doc.i

%include openturns/LHS.hxx
namespace OT{ %extend LHS { LHS(const LHS & other) { return new OT::LHS(other); } } }
