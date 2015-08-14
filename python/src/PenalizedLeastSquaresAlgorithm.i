// SWIG file PenalizedLeastSquaresAlgorithm.i

%{
#include "PenalizedLeastSquaresAlgorithm.hxx"
%}

%include PenalizedLeastSquaresAlgorithm_doc.i

%include PenalizedLeastSquaresAlgorithm.hxx
namespace OT{ %extend PenalizedLeastSquaresAlgorithm { PenalizedLeastSquaresAlgorithm(const PenalizedLeastSquaresAlgorithm & other) { return new OT::PenalizedLeastSquaresAlgorithm(other); } } }
