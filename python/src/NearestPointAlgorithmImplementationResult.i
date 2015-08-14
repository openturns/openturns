// SWIG file NearestPointAlgorithmImplementationResult.i

%{
#include "NearestPointAlgorithmImplementationResult.hxx"
%}

%include NearestPointAlgorithmImplementationResult.hxx
namespace OT{ %extend NearestPointAlgorithmImplementationResult { NearestPointAlgorithmImplementationResult(const NearestPointAlgorithmImplementationResult & other) { return new OT::NearestPointAlgorithmImplementationResult(other); } } }
