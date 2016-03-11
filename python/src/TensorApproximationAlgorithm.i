// SWIG file TensorApproximationAlgorithm.i

%{
#include "openturns/TensorApproximationAlgorithm.hxx"
%}

%include TensorApproximationAlgorithm_doc.i

%include openturns/TensorApproximationAlgorithm.hxx

namespace OT{ %extend TensorApproximationAlgorithm { TensorApproximationAlgorithm(const TensorApproximationAlgorithm & other) { return new OT::TensorApproximationAlgorithm(other); } } }
