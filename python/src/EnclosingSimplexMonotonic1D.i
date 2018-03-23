// SWIG file EnclosingSimplexMonotonic1D.i

%{
#include "openturns/EnclosingSimplexMonotonic1D.hxx"
%}

%include EnclosingSimplexMonotonic1D_doc.i

%include openturns/EnclosingSimplexMonotonic1D.hxx
namespace OT { %extend EnclosingSimplexMonotonic1D { EnclosingSimplexMonotonic1D(const EnclosingSimplexMonotonic1D & other) { return new OT::EnclosingSimplexMonotonic1D(other); } } }
