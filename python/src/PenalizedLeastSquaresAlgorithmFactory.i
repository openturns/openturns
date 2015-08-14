// SWIG file PenalizedLeastSquaresAlgorithmFactory.i

%{
#include "PenalizedLeastSquaresAlgorithmFactory.hxx"
%}

%include PenalizedLeastSquaresAlgorithmFactory_doc.i

%include PenalizedLeastSquaresAlgorithmFactory.hxx
namespace OT { %extend PenalizedLeastSquaresAlgorithmFactory { PenalizedLeastSquaresAlgorithmFactory(const PenalizedLeastSquaresAlgorithmFactory & other) { return new OT::PenalizedLeastSquaresAlgorithmFactory(other); } } }
