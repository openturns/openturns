// SWIG file ApproximationAlgorithmImplementation.i

%{
#include "openturns/ApproximationAlgorithmImplementation.hxx"
%}

%include ApproximationAlgorithmImplementation_doc.i

%ignore OT::ApproximationAlgorithmImplementation::getSelectionHistory;

%include openturns/ApproximationAlgorithmImplementation.hxx
namespace OT { %extend ApproximationAlgorithmImplementation { ApproximationAlgorithmImplementation(const ApproximationAlgorithmImplementation & other) { return new OT::ApproximationAlgorithmImplementation(other); } } }
