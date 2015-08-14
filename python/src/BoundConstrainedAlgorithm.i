// SWIG file BoundConstrainedAlgorithm.i

%{
#include "BoundConstrainedAlgorithm.hxx"
%}

OTTypedInterfaceObjectHelper(BoundConstrainedAlgorithm)

%include BoundConstrainedAlgorithm.hxx

namespace OT{ %extend BoundConstrainedAlgorithm { BoundConstrainedAlgorithm(const BoundConstrainedAlgorithm & other) { return new OT::BoundConstrainedAlgorithm(other); } } }
