// SWIG file PenalizedLeastSquaresAlgorithm.i

%{
#include "openturns/PenalizedLeastSquaresAlgorithm.hxx"
%}

%include PenalizedLeastSquaresAlgorithm_doc.i

%include openturns/PenalizedLeastSquaresAlgorithm.hxx
namespace OT{ %extend PenalizedLeastSquaresAlgorithm { PenalizedLeastSquaresAlgorithm(const PenalizedLeastSquaresAlgorithm & other) { return new OT::PenalizedLeastSquaresAlgorithm(other); } } }
