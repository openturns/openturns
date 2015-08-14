// SWIG file NearestPointAlgorithmImplementation.i

%{
#include "NearestPointAlgorithmImplementation.hxx"
%}

%include NearestPointAlgorithmImplementation.hxx
namespace OT{ %extend NearestPointAlgorithmImplementation { NearestPointAlgorithmImplementation(const NearestPointAlgorithmImplementation & other) { return new OT::NearestPointAlgorithmImplementation(other); } } }
