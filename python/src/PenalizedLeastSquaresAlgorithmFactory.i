// SWIG file PenalizedLeastSquaresAlgorithmFactory.i

%{
#include "openturns/PenalizedLeastSquaresAlgorithmFactory.hxx"
%}

%include PenalizedLeastSquaresAlgorithmFactory_doc.i

%include openturns/PenalizedLeastSquaresAlgorithmFactory.hxx
namespace OT { %extend PenalizedLeastSquaresAlgorithmFactory { PenalizedLeastSquaresAlgorithmFactory(const PenalizedLeastSquaresAlgorithmFactory & other) { return new OT::PenalizedLeastSquaresAlgorithmFactory(other); } } }
