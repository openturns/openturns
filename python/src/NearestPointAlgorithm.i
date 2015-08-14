// SWIG file NearestPointAlgorithm.i

%{
#include "NearestPointAlgorithm.hxx"
%}

OTTypedInterfaceObjectHelper(NearestPointAlgorithm)

%include NearestPointAlgorithm.hxx

namespace OT{ %extend NearestPointAlgorithm { NearestPointAlgorithm(const NearestPointAlgorithm & other) { return new OT::NearestPointAlgorithm(other); } } }
