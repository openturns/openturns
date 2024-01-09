// SWIG file ApproximationAlgorithmImplementation.i

%{
#include "openturns/ApproximationAlgorithmImplementation.hxx"
%}

%include ApproximationAlgorithmImplementation_doc.i

%ignore OT::ApproximationAlgorithmImplementation::getSelectionHistory;
%ignore OT::ApproximationAlgorithmImplementation::getErrorHistory;

%copyctor OT::ApproximationAlgorithmImplementation;

%include openturns/ApproximationAlgorithmImplementation.hxx
