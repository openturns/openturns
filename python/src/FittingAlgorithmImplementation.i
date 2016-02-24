// SWIG file FittingAlgorithmImplementation.i

%{
#include "openturns/FittingAlgorithmImplementation.hxx"
%}

%include FittingAlgorithmImplementation_doc.i

%include openturns/FittingAlgorithmImplementation.hxx
namespace OT { %extend FittingAlgorithmImplementation { FittingAlgorithmImplementation(const FittingAlgorithmImplementation & other) { return new OT::FittingAlgorithmImplementation(other); } } }
