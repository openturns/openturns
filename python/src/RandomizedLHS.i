// SWIG file RandomizedLHS.i

%{
#include "openturns/RandomizedLHS.hxx"
%}

%include RandomizedLHS_doc.i

%include openturns/RandomizedLHS.hxx
namespace OT { %extend RandomizedLHS { RandomizedLHS(const RandomizedLHS & other) { return new OT::RandomizedLHS(other); } } }
