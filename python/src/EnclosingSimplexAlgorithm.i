// SWIG file EnclosingSimplexAlgorithm.i

%{
#include "openturns/EnclosingSimplexAlgorithm.hxx"
%}

%include EnclosingSimplexAlgorithm_doc.i

OTTypedInterfaceObjectHelper(EnclosingSimplexAlgorithm)

%include openturns/EnclosingSimplexAlgorithm.hxx
namespace OT{ %extend EnclosingSimplexAlgorithm { EnclosingSimplexAlgorithm(const EnclosingSimplexAlgorithm & other) { return new OT::EnclosingSimplexAlgorithm(other); } } }
