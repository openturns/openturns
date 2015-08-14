// SWIG file FittingAlgorithmImplementation.i

%{
#include "FittingAlgorithmImplementation.hxx"
%}

%include FittingAlgorithmImplementation_doc.i

%include FittingAlgorithmImplementation.hxx
namespace OT { %extend FittingAlgorithmImplementation { FittingAlgorithmImplementation(const FittingAlgorithmImplementation & other) { return new OT::FittingAlgorithmImplementation(other); } } }
