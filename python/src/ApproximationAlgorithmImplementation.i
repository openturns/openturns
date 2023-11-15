// SWIG file ApproximationAlgorithmImplementation.i

%{
#include "openturns/ApproximationAlgorithmImplementation.hxx"
%}

%include ApproximationAlgorithmImplementation_doc.i

%ignore OT::ApproximationAlgorithmImplementation::getSelectionHistory;

%copyctor OT::ApproximationAlgorithmImplementation;

%include openturns/ApproximationAlgorithmImplementation.hxx
