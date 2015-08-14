// SWIG file BoundConstrainedAlgorithmImplementationResult.i

%{
#include "BoundConstrainedAlgorithmImplementationResult.hxx"
%}

%include BoundConstrainedAlgorithmImplementationResult.hxx
namespace OT{ %extend BoundConstrainedAlgorithmImplementationResult { BoundConstrainedAlgorithmImplementationResult(const BoundConstrainedAlgorithmImplementationResult & other) { return new OT::BoundConstrainedAlgorithmImplementationResult(other); } } }
