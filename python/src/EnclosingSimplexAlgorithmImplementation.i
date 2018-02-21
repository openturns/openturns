// SWIG file EnclosingSimplexAlgorithmImplementation.i

%{
#include "openturns/EnclosingSimplexAlgorithmImplementation.hxx"
%}

%include EnclosingSimplexAlgorithmImplementation_doc.i

%include openturns/EnclosingSimplexAlgorithmImplementation.hxx
namespace OT{ %extend EnclosingSimplexAlgorithmImplementation { EnclosingSimplexAlgorithmImplementation(const EnclosingSimplexAlgorithmImplementation & other) { return new OT::EnclosingSimplexAlgorithmImplementation(other); } } }
