// SWIG file BoundConstrainedAlgorithmImplementation.i

%{
#include "BoundConstrainedAlgorithmImplementation.hxx"
%}

%include BoundConstrainedAlgorithmImplementation.hxx
namespace OT{ %extend BoundConstrainedAlgorithmImplementation { BoundConstrainedAlgorithmImplementation(const BoundConstrainedAlgorithmImplementation & other) { return new OT::BoundConstrainedAlgorithmImplementation(other); } } }
