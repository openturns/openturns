// SWIG file TensorApproximationResult.i

%{
#include "openturns/TensorApproximationResult.hxx"
%}

%include TensorApproximationResult_doc.i

%include openturns/TensorApproximationResult.hxx

namespace OT{ %extend TensorApproximationResult { TensorApproximationResult(const TensorApproximationResult & other) { return new OT::TensorApproximationResult(other); } } }
