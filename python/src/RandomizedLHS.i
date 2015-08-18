// SWIG file RandomizedLHS.i

%{
#include "RandomizedLHS.hxx"
%}

%include RandomizedLHS_doc.i

%include RandomizedLHS.hxx
namespace OT { %extend RandomizedLHS { RandomizedLHS(const RandomizedLHS & other) { return new OT::RandomizedLHS(other); } } }
