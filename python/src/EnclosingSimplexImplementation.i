// SWIG file EnclosingSimplexImplementation.i

%{
#include "openturns/EnclosingSimplexImplementation.hxx"
%}

%include EnclosingSimplexImplementation_doc.i

%include openturns/EnclosingSimplexImplementation.hxx
namespace OT{ %extend EnclosingSimplexImplementation { EnclosingSimplexImplementation(const EnclosingSimplexImplementation & other) { return new OT::EnclosingSimplexImplementation(other); } } }
